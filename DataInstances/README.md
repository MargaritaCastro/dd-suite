
# DataInstances — Benchmark Instance Datasets

This folder contains all the datasets used for testing and benchmarking the algorithms across different problems. It is organized into three main problem types:

- `IndependentSet`
- `Knapsack`
- `SetCover`
- `SOCKnapsack`

Each of the first three contains three subfolders: `Custom`, `Instances_generation`, and `Standard`. `SOCKnapsack` contains instance files directly.

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
│   └── *.txt
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

Instances for the Second-Order Cone Knapsack problem. Files are named following the pattern `knapsack_nNmMoObB_seed.txt`, where `N` is the number of variables, `M` the number of constraints, `O` the omega scaling factor, and `B` the RHS bound. A `medium/` subfolder contains larger instances.

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

- `n = [250, 500, ..., 4000]`
- `d = [0.1]`
- `bandwidth = [165]`

Implemented in:`SetCover/Instances_generation/matrix_creation.py`

> You can modify `n`, `d`, or `bandwidth` in the script to generate new instances.

> After generation, move new files to `Standard/` to make them usable in benchmarking workflows.

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
python matrix_creation.py
```

Each script includes editable parameters at the top (e.g., number of variables, density, bandwidth) which you can customize before running the script.
