"""
Converter from TSPLIB ATSP instances to the 1|s_ij|Σw_j·C_j format
(single machine, sequence-dependent setup times, minimize weighted completion time)

Source: https://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/  (atsp/ folder)

Mapping
───────
The asymmetric ATSP distance matrix is used as the setup time matrix. City 0 is
taken as the depot and cities 1..n-1 become jobs 0..n-2, so that a job sequence
is an open Hamiltonian path starting at the depot (our model does not close the
cycle):

    setup_times[0][j]     = d(depot, job j)         <- "from depot" row
    setup_times[i+1][j]   = d(job i, job j)
    setup_times[i+1][i]   = 0                       <- diagonal, arc never taken

The original TSPLIB diagonal carries a big-M (9999, 9999999 or 1e8 depending on
the instance) and lands exactly on those unused positions. It is KEPT rather than
zeroed: no feasible sequence ever traverses job -> itself, but a relaxed DD can,
and if that arc is cheap the relaxation degenerates into scheduling a single job
over and over, which collapses the bound (with a zeroed diagonal, ftv33 spent 31
of its 33 arcs looping on one job). Some TSPLIB diagonals are 0 on part of the
entries, so the value written is max(diagonal, min_entering[j]), where
min_entering[j] is the cheapest setup entering j from any other predecessor.

A big-M appearing OFF the diagonal would mean a forbidden arc, which our model
cannot express: in that case the instance is skipped with a warning.

TSPLIB provides no processing times or weights, so they are drawn with the same
scheme as generate_instances.py:

    p_j ~ U[1, 10]        w_j ~ U[1, 10]

Several instances are produced per ATSP file (same matrix, different p and w),
with reproducible seeds derived from the instance name.

IMPORTANT
─────────
The TSPLIB best known values (ftv33 = 1286, ry48p = 14422, etc.) are NOT
comparable to the values reported by our solvers: TSPLIB minimizes the length of
a closed tour (every arc with coefficient 1) whereas we minimize Σ w_j·C_j,
where every arc is weighted by the total weight still pending. Use main_gurobi
(SequencingGurobiClass) to validate, not the best known values.

Usage
─────
  python atsp_to_sequencing.py

Reads ATSP/*.atsp (and .atsp.gz) and writes the converted instances to Standard/.
"""

import gzip
import os
import random
import re

# ── configuration ─────────────────────────────────────────────────────────────

MAX_CITIES = 50            # keep only instances with fewer than 50 cities
SEEDS_PER_INSTANCE = 5     # instances generated per ATSP file (p and w vary)

P_MIN, P_MAX = 1, 10       # processing time range
W_MIN, W_MAX = 1, 10       # weight range

BIG_M = 1_000_000          # threshold used to detect the TSPLIB "infinity"

# ── TSPLIB parsing ────────────────────────────────────────────────────────────

def read_atsp(path: str) -> tuple[int, list[list[int]]]:
    """
    Return (n_cities, n x n matrix). Only EXPLICIT / FULL_MATRIX is supported,
    which is the format of every ATSP instance in TSPLIB.
    """
    opener = gzip.open if path.endswith(".gz") else open
    with opener(path, "rt") as f:
        text = f.read()

    def field(key: str) -> str | None:
        match = re.search(rf"^\s*{key}\s*:\s*(\S+)", text, re.MULTILINE)
        return match.group(1) if match else None

    weight_type = field("EDGE_WEIGHT_TYPE")
    weight_format = field("EDGE_WEIGHT_FORMAT")
    if weight_type != "EXPLICIT" or weight_format != "FULL_MATRIX":
        raise ValueError(f"unsupported format: {weight_type}/{weight_format}")

    n = int(field("DIMENSION"))

    body = text.split("EDGE_WEIGHT_SECTION", 1)[1]
    body = re.split(r"[A-Z][A-Z_]*_SECTION|EOF", body)[0]
    values = [int(v) for v in body.split()]
    if len(values) < n * n:
        raise ValueError(f"expected {n * n} weights but read {len(values)}")

    return n, [values[i * n:(i + 1) * n] for i in range(n)]

# ── conversion ────────────────────────────────────────────────────────────────

def build_setup_times(n: int, matrix: list[list[int]]) -> tuple[int, list[list[int]]]:
    """
    City 0 = depot, cities 1..n-1 = jobs 0..n-2.
    Returns (n_jobs, setup_times) with setup_times of size (n_jobs + 1) x n_jobs.
    """
    n_jobs = n - 1

    # Lower bound on the setup paid when entering job j from a DIFFERENT predecessor
    # (the depot or any other job). A feasible sequence never enters j from j itself,
    # so this value can never alter a real arc.
    min_entering = [min(matrix[i][j + 1] for i in range(n) if i != j + 1)
                    for j in range(n_jobs)]

    setup_times = [[matrix[0][j + 1] for j in range(n_jobs)]]
    for i in range(n_jobs):
        row = []
        for j in range(n_jobs):
            if i == j:
                # Diagonal: the arc job i -> job i does not exist in a feasible
                # sequence, but a relaxed DD can traverse it, and if it is the
                # cheapest arc around the bound collapses into scheduling one job
                # over and over. TSPLIB stores a big-M here, which is exactly what
                # we want; where it stores 0 instead, fall back to min_entering.
                row.append(max(matrix[i + 1][j + 1], min_entering[j]))
            else:
                row.append(matrix[i + 1][j + 1])
        setup_times.append(row)

    return n_jobs, setup_times


def find_forbidden_arcs(setup_times: list[list[int]]) -> list[tuple[int, int]]:
    '''
    Off-diagonal positions holding a big-M, i.e. arcs that TSPLIB forbids and that
    our model cannot represent. The diagonal is skipped: it deliberately holds a
    large value so that a relaxed DD never prefers the (infeasible) job -> itself arc.
    '''
    return [(i, j)
            for i, row in enumerate(setup_times)
            for j, value in enumerate(row)
            if i != j + 1 and value >= BIG_M]


def generate_jobs(n_jobs: int, seed_key: str) -> tuple[list[int], list[int]]:
    rng = random.Random(seed_key)
    p = [rng.randint(P_MIN, P_MAX) for _ in range(n_jobs)]
    w = [rng.randint(W_MIN, W_MAX) for _ in range(n_jobs)]
    return p, w

# ── writing ───────────────────────────────────────────────────────────────────

def write_instance(path: str, n_jobs: int, p: list[int], w: list[int],
                   setup_times: list[list[int]]) -> None:
    with open(path, "w") as f:
        f.write(f"{n_jobs}\n")
        f.write(" ".join(map(str, p)) + "\n")
        f.write(" ".join(map(str, w)) + "\n")
        for row in setup_times:
            f.write(" ".join(map(str, row)) + "\n")

# ── main ──────────────────────────────────────────────────────────────────────

if __name__ == "__main__":
    base_dir = os.path.dirname(os.path.abspath(__file__))
    atsp_dir = os.path.join(base_dir, "ATSP")
    out_dir = os.path.join(base_dir, os.pardir, "Standard")   # Sequencing/Standard
    os.makedirs(out_dir, exist_ok=True)

    files = sorted(f for f in os.listdir(atsp_dir)
                   if f.endswith(".atsp") or f.endswith(".atsp.gz"))

    converted, too_big, skipped = [], [], []

    for filename in files:
        name = filename.split(".atsp")[0]
        try:
            n, matrix = read_atsp(os.path.join(atsp_dir, filename))
        except ValueError as error:
            skipped.append((name, str(error)))
            continue

        if n >= MAX_CITIES:
            too_big.append((name, n))
            continue

        n_jobs, setup_times = build_setup_times(n, matrix)

        forbidden = find_forbidden_arcs(setup_times)
        if forbidden:
            skipped.append((name, f"{len(forbidden)} forbidden arcs (big-M) off the diagonal"))
            continue

        for seed in range(1, SEEDS_PER_INSTANCE + 1):
            p, w = generate_jobs(n_jobs, f"{name}-{seed}")
            out_name = f"sequencing_atsp_{name}_n{n_jobs}_seed{seed}.txt"
            write_instance(os.path.join(out_dir, out_name), n_jobs, p, w, setup_times)

        setups = [value for i, row in enumerate(setup_times)
                  for j, value in enumerate(row) if i != j + 1]
        converted.append((name, n, n_jobs, min(setups), max(setups)))

    print(f"Instances written to {out_dir}/\n")
    print(f"{'instance':<12}{'cities':>10}{'jobs':>7}{'setup min':>11}{'setup max':>11}{'files':>10}")
    for name, n, n_jobs, s_min, s_max in converted:
        print(f"{name:<12}{n:>10}{n_jobs:>7}{s_min:>11}{s_max:>11}{SEEDS_PER_INSTANCE:>10}")
    print(f"\nTotal: {len(converted)} ATSP instances -> "
          f"{len(converted) * SEEDS_PER_INSTANCE} files "
          f"(seeds 1..{SEEDS_PER_INSTANCE}, p and w ~ U[1,10])")

    if too_big:
        print(f"\nSkipped by size (>= {MAX_CITIES} cities): "
              + ", ".join(f"{name}({n})" for name, n in too_big))
    if skipped:
        print("\nSkipped by format or forbidden arcs:")
        for name, reason in skipped:
            print(f"  {name}: {reason}")

    print("\nReminder: the TSPLIB best known values are not comparable to "
          "Σw_j·C_j; validate with main_gurobi.")
