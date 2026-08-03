# data_visualization — Plot and Table Construction

This folder contains everything related to **the construction of the plots and tables** used in the paper, after the experimental tests have been executed.

Only the scripts that generate the figures and tables included in the paper are kept here. The CSVs under `data/cpp/` and `data/python/` are produced by the `DD_c_plusplus` and `DD_python` implementations, and those under `data/rust/` by `ddo_simplificado` (a simplified copy of the [DDO library](https://github.com/xgillard/ddo)).

Only the **latest version** of each result file and of each generated plot or table is kept in this repository.

---

## Folder Structure

```
data_visualization/
├── data/
│   ├── cpp/               # CSV result files from the C++ implementation
│   │   ├── results_cpp_*.csv           # main DD experiments (relax priority + grouping share these rows)
│   │   ├── results_cpp_grouping_*.csv  # relax grouping experiments
│   │   ├── soc_cuts_*.csv              # SOC Knapsack cut experiments
│   │   └── soc_gurobi_*.csv            # SOC Knapsack Gurobi baseline
│   ├── python/            # CSV result files from the Python implementation
│   │   ├── results_python_*.csv
│   │   ├── results_python_grouping_*.csv
│   │   ├── results_gurobi_python_*.csv # Gurobi baseline (used by grouping_vs_priority)
│   │   ├── soc_cuts_*.csv
│   │   └── soc_gurobi_*.csv
│   └── rust/              # CSV result files from the Rust baseline
│       └── results_rust_*.csv
├── plots/                                 # Output PNG plots (one subfolder per script)
│   ├── dd_construction_time_comparison/   # from dd_construction_time_comparison.py
│   ├── rust_construction_time/            # from rust_plots.py
│   └── grouping_vs_priority/              # from grouping_vs_priority_plots.py
├── tables/                                # Output PDF/PNG tables (one subfolder per script)
│   ├── python_cpp_agreement_table/        # from python_cpp_agreement_table.py
│   ├── construction_time_ratio_table/     # from construction_time_ratio_table.py
│   └── soc_cuts_summary_table/            # from soc_cuts_summary_table.py
├── dd_construction_time_comparison.py  # C++ vs Python construction time scatter plots
├── rust_plots.py                       # Rust vs C++ and Rust vs Python scatter plots
├── grouping_vs_priority_plots.py       # Relax grouping vs relax priority (time & gap)
├── python_cpp_agreement_table.py       # Python vs C++ agreement table PDF
├── construction_time_ratio_table.py    # Average construction-time ratio per DD type (paper table)
├── soc_cuts_summary_table.py           # SOC Knapsack cut summary table PNG
└── readme.md                           # This file
```

Each script automatically loads the **most recently timestamped** CSV file matching the expected pattern, so you only need to drop new CSVs into the appropriate subfolder.

Each script writes to **its own subfolder** under `plots/` or `tables/`, and before generating new outputs it moves any existing files in that subfolder into a local `deprecated/` (so the subfolder root always holds only the latest run). Those `deprecated/` folders are local history and are not kept in this repository.

---

## CSV Format

### DD results (`results_cpp_*.csv`, `results_python_*.csv`, `results_rust_*.csv`)

Must include at minimum:

```
"File_name", "Problem type", "Variables number", "DD Type", "Max Width",
"Is Reduced", "Objective Type", "Optimization Value", "Construction Time",
"Reduce Time", "Solution Time", "Number Node", "Number Arcs", "Sort"
```

### Gurobi results (`results_gurobi_python_*.csv`)

Must include: `"File_name"`, `"Problem type"`, `"Variables number"`, `"Best solution"`.

### SOC Knapsack cut results (`soc_cuts_*.csv`)

Must include: `"File_name"`, `"Cut Type"`, `"CutStrengthening"`, `"Solution Time"`, `"DD Time"`, `"Gap"`, `"NodeBB"`, `"Num Cuts"`.

### SOC Knapsack Gurobi results (`soc_gurobi_*.csv`)

Must include: `"File_name"`, `"Solution Time"`, `"Gap"`, `"NodeBB"`.

---

## Plot Scripts

### `dd_construction_time_comparison.py`

Compares **C++ vs Python** construction times for standard DD experiments. Produces the `construction_time_*` figures of the paper.

#### Key parameters:

```python
RELAX_TYPE = "priority"      # which relax the *_grouping_* CSVs select (legacy switch)
missing_time = 310           # Time assigned to timed-out tests
min_variables_default = 100  # Minimum variable count to include
min_variables_by_problem = {'SOC Knapsack': 0, 'Sequencing': 0}  # Per-problem overrides
EXCLUDED_PROBLEM_TYPES = ['SOC Knapsack']
csv_folder = './data'        # Root data folder
```

#### What it generates (`./plots/dd_construction_time_comparison/`):

- **One main scatter plot per DD type** present in the CSV (currently `Exact`, `Restricted`, `RelaxPriority` and `RelaxGrouping`), comparing C++ vs Python construction time on a log scale. Points are colored by problem type.
- **One secondary plot per (width-dependent DD type) × (problem type)** — grouping by `Max Width` to show the effect of width on construction time.

Named: `construction_time_<dd_type>_<timestamp>.png` (and `construction_time_<dd_type>_<problem_type>_<timestamp>.png` for the secondary plots).

---

### `rust_plots.py`

Compares **Rust vs C++** and **Rust vs Python** construction times, using the results produced by `ddo_simplificado` and stored in `data/rust/`. Produces the `comparison_rust_cpp_*` figures of the paper.

#### Key parameters:

```python
RELAX_TYPE = "priority"      # ddo implements a single relax; C++/Python are reduced to it
missing_time = 310
min_variables_default = 100
min_variables_by_problem = {'SOC Knapsack': 0, 'Sequencing': 0}
EXCLUDED_PROBLEM_TYPES = ['SOC Knapsack']
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

## Table Scripts

### `python_cpp_agreement_table.py`

Generates a **PDF table** measuring how often the Python and C++ implementations produce the same diagram, broken down by problem (rows) and DD type (columns).

Configurations are matched on the key `(File_name, Problem type, DD Type, Max Width, Is Reduced, Objective Type, Sort)`, and only configurations present in **both** CSVs are counted; the coverage difference (rows present in one language only, typically due to time limits) is reported in the header.

#### Metrics compared (one matrix each):

- **Objective value** (`Optimization Value`) — compared with a relative tolerance of `1e-4`, since C++ writes floating-point (`253.000000`) and Python writes integers (`253`).
- **Node count** (`Number Node`) — exact integer match.
- **Arc count** (`Number Arcs`) — exact integer match.

#### What it generates (`./tables/python_cpp_agreement_table/`):

- **`python_cpp_agreement_<timestamp>.pdf`** — three matrices with problems on the rows and DD types on the columns, plus a total row and column. Each cell reads `matching / comparable` and is color-coded from green (100% agreement) through yellow and orange to red as the agreement drops.

---

### `construction_time_ratio_table.py`

Generates the **average construction-time ratio table (PNG)** of the paper (Table `tab:efficiency`), i.e. the same numbers summarized by the scatter plots of `dd_construction_time_comparison.py` (Python vs C++) and `rust_plots.py` (C++ vs `ddo`). A ratio of *k* means the slower implementation takes *k* times longer.

#### Key parameters:

```python
RELAX_TYPE = "priority"      # relax used for C++/Python (ddo only implements one)
TIME_LIMIT_SECONDS = 300     # 5-min construction limit used in the experiments
min_variables_default = 100
min_variables_by_problem = {'SOC Knapsack': 0, 'Sequencing': 0}
EXCLUDED_PROBLEM_TYPES = ['SOC Knapsack']
EXCLUDED_INSTANCES_RUST = ['set_cover_n4000_m3836_d0.02_b_w165_seed1']
```

The average is the arithmetic mean of the ratio over every (instance, width) pair, per DD type. Pairs where either implementation hits the 5-minute limit are **excluded** (they only appear in one CSV, or with a censored time, and would bias the mean). In the C++ / `ddo` block, `set_cover_n4000_m3836_d0.02_b_w165_seed1` is also excluded: `ddo` returns a degenerate width-1 DD in ~0.015 s for every width, which on its own pushed the restricted/relaxed average from ~2.5 to ~7. That block compares against the non-reduced C++ rows, since `ddo` does not implement reduction.

#### What it generates (`./tables/construction_time_ratio_table/`):

- **`construction_time_ratio_<timestamp>.png`** — table with one row per (comparison, DD type): Python / C++ and C++ / `ddo`, each for `Exact`, `Restricted` and `Relaxed`. Above the table, a header reports which CSV each implementation came from and, per comparison, the number of common pairs averaged plus the configurations present in one implementation only (the "only" counts, i.e. the runs that did not finish within the time limit).

The console output additionally reports the number of pairs behind each average and a per-problem breakdown, useful to check where each average comes from. The step-by-step pipeline is documented as a comment at the bottom of the script.

---

### `soc_cuts_summary_table.py`

Generates a **summary table (PNG)** for SOC Knapsack cut experiments comparing C++ and Python implementations across all cut variants.

#### Cut variants covered:

`FlowCuts`, `FlowCuts + Strengthening`, `JointFlowCuts`, `JointFlowCuts + Strengthening` and `TargetCuts` (plus a Gurobi baseline row). TargetCuts is not strengthened: it already yields facet-defining inequalities.

#### Metrics per variant and language:

| Metric | Description |
|--------|-------------|
| `# Solv` | Instances of that language solved to optimality (`Gap == 0`); the only metric **not** restricted to the common set |
| `Gap (%)` | Average MIP gap over the **unsolved** instances |
| `Time (s)` | Average time over the **solved** instances |
| `NodeBB` | Average B&B nodes explored |
| `# Cuts` | Average cuts generated |

All metrics except `# Solv` are averaged over the instances present in both languages for that same method.

#### What it generates (`./tables/soc_cuts_summary_table/`):

- **`soc_cuts_summary_<timestamp>.png`** — side-by-side table for C++ and Python.

---

## How to Run

From within the `data_visualization/` directory:

```bash
python3 dd_construction_time_comparison.py
python3 rust_plots.py
python3 grouping_vs_priority_plots.py
python3 python_cpp_agreement_table.py
python3 construction_time_ratio_table.py
python3 soc_cuts_summary_table.py
```

All scripts auto-select the latest CSV file matching the expected pattern in each subfolder.

---

## Output Management

- Each script writes to **its own subfolder** under `plots/` or `tables/`, so outputs never mix between scripts.
- Before generating new outputs, every script moves the existing files in its subfolder into a local **`deprecated/`** — the subfolder root always holds just the latest run.
