# data_visualization — Plot Construction

This folder contains everything related to **the construction of the plots** used in the paper, after the experimental tests have been executed.

Only the three scripts that generate the figures included in the paper are kept here. The CSVs under `data/cpp/` and `data/python/` are produced by the `DD_c_plusplus` and `DD_python` implementations, and those under `data/rust/` by `ddo_simplificado` (a simplified copy of the [DDO library](https://github.com/xgillard/ddo)).

Only the **latest version** of each result file and of each generated plot is kept in this repository.

---

## Folder Structure

```
data_visualization/
├── data/
│   ├── cpp/               # CSV result files from the C++ implementation
│   │   ├── results_cpp_*.csv           # main DD experiments (relax priority + grouping share these rows)
│   │   ├── results_cpp_grouping_*.csv  # relax grouping experiments
│   │   └── results_scheduler_cpp_*.csv # Scheduler experiments
│   ├── python/            # CSV result files from the Python implementation
│   │   ├── results_python_*.csv
│   │   ├── results_python_grouping_*.csv
│   │   ├── results_gurobi_python_*.csv # Gurobi baseline (used by grouping_vs_priority)
│   │   └── results_scheduler_python_*.csv
│   └── rust/              # CSV result files from the Rust baseline
│       └── results_rust_*.csv
├── plots/                                 # Output PNG plots (one subfolder per script)
│   ├── dd_construction_time_comparison/   # from dd_construction_time_comparison.py
│   ├── rust_construction_time/            # from rust_plots.py
│   └── grouping_vs_priority/              # from grouping_vs_priority_plots.py
├── dd_construction_time_comparison.py  # C++ vs Python construction time scatter plots
├── rust_plots.py                       # Rust vs C++ and Rust vs Python scatter plots
├── grouping_vs_priority_plots.py       # Relax grouping vs relax priority (time & gap)
└── readme.md                           # This file
```

Each script automatically loads the **most recently timestamped** CSV file matching the expected pattern, so you only need to drop new CSVs into the appropriate subfolder.

Each script writes to **its own subfolder** under `plots/`, and before generating new outputs it moves any existing files in that subfolder into a local `deprecated/` (so the subfolder root always holds only the latest run). Those `deprecated/` folders are local history and are not kept in this repository.

---

## CSV Format

### DD results (`results_cpp_*.csv`, `results_python_*.csv`, `results_rust_*.csv`)

Must include at minimum:

```
"File_name", "Problem type", "Variables number", "DD Type", "Max Width",
"Is Reduced", "Objective Type", "Optimization Value", "Construction Time",
"Reduce Time", "Solution Time"
```

### Gurobi results (`results_gurobi_python_*.csv`)

Must include: `"File_name"`, `"Problem type"`, `"Variables number"`, `"Best solution"`.

---

## Plot Scripts

### `dd_construction_time_comparison.py`

Compares **C++ vs Python** construction times for standard DD experiments. Produces the `construction_time_*` figures of the paper.

#### Key parameters:

```python
missing_time = 310           # Time assigned to timed-out tests
min_variables_default = 100  # Minimum variable count to include
min_variables_by_problem = {'SOC Knapsack': 0}  # Per-problem overrides
csv_folder = './data'        # Root data folder
```

#### What it generates (`./plots/dd_construction_time_comparison/`):

- **3 main scatter plots** — one per DD type (`Exact`, `RelaxPriority`, `Restricted`), comparing C++ vs Python construction time on a log scale. Points are colored by problem type.
- **Secondary plots** — one per (relaxation/restricted) × (problem type) combination, grouping by `Max Width` to show the effect of width on construction time.

Named: `construction_time_<dd_type>_<timestamp>.png` (and `construction_time_<dd_type>_<problem_type>_<timestamp>.png` for the secondary plots).

---

### `rust_plots.py`

Compares **Rust vs C++** and **Rust vs Python** construction times, using the results produced by `ddo_simplificado` and stored in `data/rust/`. Produces the `comparison_rust_cpp_*` figures of the paper.

#### Key parameters:

```python
missing_time = 310
min_variables_default = 100
min_variables_by_problem = {'SOC Knapsack': 0}
```

#### What it generates (`./plots/rust_construction_time/`):

- **6 scatter plots** — for each DD type (`Exact`, `Relaxed`, `Restricted`), two plots: Rust vs C++ and Rust vs Python.

Named: `comparison_rust_cpp_<dd_type>_<timestamp>.png` and `comparison_rust_python_<dd_type>_<timestamp>.png`.

---

### `grouping_vs_priority_plots.py`

Compares the two relaxation strategies — **relax grouping vs relax priority** — for C++ and Python. Both variants live in the same `results_<lang>_*.csv` files and are told apart by the `DD Type` column. Produces the `grouping_vs_priority_*` figures of the paper.

#### What it generates (`./plots/grouping_vs_priority/`):

- **Gap plots** — one per (language × problem type), comparing the bound gap of both strategies.
- **Time plots** — one per language, comparing construction time.

Named: `grouping_vs_priority_gap_<lang>_<problem_type>_<timestamp>.png` and `grouping_vs_priority_time_<lang>_<timestamp>.png`.

---

## How to Run

From within the `data_visualization/` directory:

```bash
python3 dd_construction_time_comparison.py
python3 rust_plots.py
python3 grouping_vs_priority_plots.py
```

All scripts auto-select the latest CSV file matching the expected pattern in each subfolder.

---

## Output Management

- Each script writes to **its own subfolder** under `plots/`, so outputs never mix between scripts.
- Before generating new outputs, every script moves the existing files in its subfolder into a local **`deprecated/`** — the subfolder root always holds just the latest run.
