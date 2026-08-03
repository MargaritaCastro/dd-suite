"""
Generator of instances for 1|s_ij|Σw_j·C_j
(single machine, sequence-dependent setup times, minimize weighted completion time)

Generation scheme follows the standard used in the sequencing literature
(Chang et al. 1995, Cicirello 2003) cited by Cirè & van Hoeve (2013):

  p_j  ~ U[1, 10]                          processing times
  w_j  ~ U[1, 10]                          weights (urgency)
  s_ij ~ U[0, ceil(sigma * mean(p))]       setup times, sigma = severity factor
         with s_ii = 0  (diagonal)
         row 0 = from depot (same distribution)

Parameters:
  n      : number of jobs
  sigma  : setup severity in {0.25, 0.5, 1.0}
           0.25 → light setups  (~25% of avg processing time)
           0.50 → medium setups (~50%)
           1.00 → heavy setups  (~100%)
  seed   : random seed for reproducibility

Output format (SequencingInstance-compatible):
  n_jobs
  p_0 p_1 ... p_{n-1}
  w_0 w_1 ... w_{n-1}
  s_00 s_01 ... s_0{n-1}    <- from depot
  s_10 s_11 ... s_1{n-1}    <- from job 0
  ...
  s_n0  ...  s_n{n-1}       <- from job n-1

Usage:
  python generate_instances.py
"""

import random
import math
import os

# ── configuration ─────────────────────────────────────────────────────────────

SIZES = [5, 10, 15, 20]          # number of jobs
SIGMAS = [0.25, 0.50, 1.00]      # setup severity factors
SEEDS_PER_CONFIG = 5             # instances per (n, sigma) combination

P_MIN, P_MAX = 1, 10             # processing time range
W_MIN, W_MAX = 1, 10             # weight range

# ── generator ─────────────────────────────────────────────────────────────────

def generate_instance(n: int, sigma: float, seed: int) -> dict:
    rng = random.Random(seed)

    p = [rng.randint(P_MIN, P_MAX) for _ in range(n)]
    w = [rng.randint(W_MIN, W_MAX) for _ in range(n)]

    mean_p = sum(p) / n
    s_max = max(1, math.ceil(sigma * mean_p))

    # setup_times: (n+1) rows x n columns
    # row 0 = from depot; rows 1..n = from job 0..n-1
    setup_times = []
    for i in range(n + 1):
        row = []
        for j in range(n):
            if i > 0 and (i - 1) == j:
                row.append(0)          # s_{jj} = 0 (same job, unused)
            else:
                row.append(rng.randint(0, s_max))
        setup_times.append(row)

    return {"n": n, "p": p, "w": w, "setup_times": setup_times,
            "sigma": sigma, "seed": seed}


def write_instance(instance: dict, path: str) -> None:
    n = instance["n"]
    p = instance["p"]
    w = instance["w"]
    s = instance["setup_times"]

    with open(path, "w") as f:
        f.write(f"{n}\n")
        f.write(" ".join(map(str, p)) + "\n")
        f.write(" ".join(map(str, w)) + "\n")
        for row in s:
            f.write(" ".join(map(str, row)) + "\n")


def sigma_tag(sigma: float) -> str:
    return f"{int(sigma * 100):03d}"     # 0.25 -> "025", 0.50 -> "050", 1.00 -> "100"


# ── main ──────────────────────────────────────────────────────────────────────

if __name__ == "__main__":
    out_dir = os.path.dirname(os.path.abspath(__file__))
    generated = 0

    for n in SIZES:
        for sigma in SIGMAS:
            for seed in range(1, SEEDS_PER_CONFIG + 1):
                inst = generate_instance(n, sigma, seed)
                tag = sigma_tag(sigma)
                filename = f"sequencing_n{n}_s{tag}_seed{seed}.txt"
                path = os.path.join(out_dir, filename)
                write_instance(inst, path)
                generated += 1

    total = len(SIZES) * len(SIGMAS) * SEEDS_PER_CONFIG
    print(f"Generated {generated}/{total} instances in {out_dir}/")
    print(f"Sizes: {SIZES}")
    print(f"Sigmas: {SIGMAS}  (setup severity)")
    print(f"Seeds: 1..{SEEDS_PER_CONFIG} per configuration")
    print()
    print("File naming: sequencing_n<n>_s<sigma>_seed<k>.txt")
    print("  n    = number of jobs")
    print("  s025 = light setups  (sigma=0.25)")
    print("  s050 = medium setups (sigma=0.50)")
    print("  s100 = heavy setups  (sigma=1.00)")
