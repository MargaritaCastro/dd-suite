# DataInstances — Benchmark Instance Datasets

This folder contains all the datasets used for testing and benchmarking the algorithms across different problems. It is organized into these problem types:

- `IndependentSet`
- `Knapsack`
- `SetCover`
- `SOCKnapsack`
- `Sequencing`

`IndependentSet`, `Knapsack`, and `SetCover` each contain three subfolders: `Custom`, `Instances_generation`, and `Standard`. `SOCKnapsack` has only a `Standard/` subfolder (plus a handful of tiny loose `.txt` files used for manual testing). `Sequencing` contains a `Standard/` subfolder with the benchmark instances used in the experiments, a `Custom/` subfolder with randomly generated ones, an `ATSP/` subfolder with the raw TSPLIB files the standard instances are derived from, and the two scripts that produce them.

---

## Folder Structure

```
DataInstances/
├── IndependentSet/
│   ├── Custom/                 # Randomly generated instances
│   ├── Instances_generation/   # Scripts to generate and convert instances
│   └── Standard/               # Standard state-of-the-art datasets
├── Knapsack/
│   ├── Custom/
│   ├── Instances_generation/
│   └── Standard/
├── SetCover/
│   ├── Custom/
│   ├── Instances_generation/
│   └── Standard/
├── SOCKnapsack/                # Second-Order Cone Knapsack instances
│   └── Standard/               # the 90-instance benchmark
├── Sequencing/
│   ├── Custom/                 
│   ├── Instances_generation/
│   └── Standard/
└── README.md
```

---

## Folder Details

### `Custom/`

Contains randomly generated instances for each problem. These are created using the script `generate_custom_PROBLEM_instances.py` located in each problem's `Instances_generation/` folder.

> ⚠️ After generation, instances must be manually moved from `Instances_generation/` to `Custom/`.

---

### `Instances_generation/`

Houses scripts for generating custom instances and converting standard datasets into the format accepted by the program.

Each problem has:

- A `generate_custom_PROBLEM_instances.py` file (to create new synthetic instances)
- Additional scripts for formatting or adapting inputs as needed

---

### `Standard/`

These folders contain **state-of-the-art benchmark instances** obtained from external sources or constructed following academic references.

---

## Problem-Specific Notes

### Knapsack

Standard instances were obtained from:
🔗 http://artemisa.unicauca.edu.co/~johnyortega/instances_01_KP/

---

### Independent Set

Used datasets from the **CODD benchmark**, adapted from the **DIMACS Graph Format** to the required input format of this project.

---

### SOCKnapsack

Instances for the Second-Order Cone Knapsack problem, in `Standard/`. Files are named following the pattern `knapsack_nNmMoObB_seed.txt`, where `N` is the number of variables, `M` the number of constraints, `O` the omega scaling factor, and `B` the RHS bound. The benchmark has 90 instances: `N ∈ {100,125,150}`, `M ∈ {10,20}`, `O ∈ {1,3,5}`, `B = 5` and 5 seeds.

---

### Set Cover

Instance generation follows the methodology in:

📖 *Decision Diagrams for Optimization* by David Bergman
See: Chapter 5.3.1 — Restricted Decision Diagrams, Problem Generation (page 88)

Instances were generated using combinations of parameters:

#### For small-scale:

- `n = 100`
- `k = [5, 10, ..., 50]`
- `d = [k[i]/100 for i in range(len(k))]`
- `bandwidth = [17, 20, ..., 38]`

#### For large-scale:

- `n = [250, 500, ..., 4000]` (16 values, in steps of 250)
- `d = 75 / n` — the script overrides `d_options` so that every row has exactly `k = 75` ones
- `bandwidth = [165]`, with `m = n - bandwidth + 1`

Implemented in: `SetCover/Instances_generation/matrix_creator.py`

> You can modify `n`, `d`, or `bandwidth` in the script to generate new instances.

> After generation, move new files to `Standard/` to make them usable in benchmarking workflows.

---

### Sequencing

Standard instances are derived from the **ATSP (Asymmetric Traveling Salesman Problem) set of TSPLIB**:
🔗 https://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/

The raw `.atsp` files are kept in `Sequencing/Instances_generation/ATSP/` and converted by `Sequencing/Instances_generation/atsp_to_sequencing.py`, which writes the results to `Sequencing/Standard/`. Only instances with **fewer than 50 cities** are converted (`br17`, `ftv33`, `ftv35`, `ftv38`, `p43`, `ftv44`, `ftv47`, `ry48p`); the larger ones present in `ATSP/` are skipped automatically.

The ATSP distance matrix supplies the sequence-dependent setup times. City 0 becomes the depot and cities 1..n-1 become the jobs, so a job sequence is an open Hamiltonian path starting at the depot and `n_jobs = n - 1`. TSPLIB provides no processing times or weights, so these are drawn as `p_j, w_j ~ U[1,10]` with a seed derived from the instance name; five instances are produced per ATSP file, named `sequencing_atsp_{name}_n{jobs}_seed{S}.txt`, giving 40 instances in total.

> The diagonal `s_jj` is **not** zeroed. No feasible sequence enters a job from itself, but a *relaxed* DD can traverse that arc, and if it is cheap the relaxation degenerates into scheduling a single job over and over (with a zeroed diagonal, `ftv33` spent 31 of its 33 arcs looping on one job and the bound dropped to 16072 instead of 56862). The value written is `max(TSPLIB diagonal, min_entering[j])`, where `min_entering[j]` is the cheapest setup entering `j` from any other predecessor — some TSPLIB diagonals are 0 on part of their entries, so the fallback is needed. This never alters a feasible arc, so exact and restricted values are unchanged. Note that the big-M sentinel on this diagonal must be excluded when computing the MIP's big-M constant, which `SequencingGurobiClass` does.

> ⚠️ The TSPLIB best known values are **not** comparable to the values our solvers report. TSPLIB minimizes the length of a closed tour (every arc with coefficient 1), whereas this problem minimizes Σ w_j·C_j, where each arc is weighted by the total job weight still pending. Use `main_gurobi` to validate optimal values.

`Custom/` still holds the randomly generated instances produced by `generate_custom_sequencing_instances.py` (`p_j, w_j ~ U[1,10]`, setup times scaled by a severity factor σ); they are kept for reference but are no longer the family used in the benchmarking workflows.

---

### Requirements and Execution

To run the Python scripts located in each `Instances_generation/` folder, you need to have **Python installed** on your system.

#### How to run the scripts

You must execute the scripts from **within** the corresponding `Instances_generation/` folder. For example:

```
cd DataInstances/Knapsack/Instances_generation
python generate_custom_knapsack_instances.py
```

or for Set Cover:

```
cd DataInstances/SetCover/Instances_generation
python matrix_creator.py
```

Each script includes editable parameters at the top (e.g., number of variables, density, bandwidth) which you can customize before running the script.
