# ddo_simplificado — Simplified version of DDO (Rust)

> ⚠️ **This is NOT the original DDO library.**
>
> This folder holds a **simplified copy** of the DDO library created by **Xavier Gillard**, whose real and official repository is:
>
> ### 👉 [https://github.com/xgillard/ddo](https://github.com/xgillard/ddo)
>
> The code has been **trimmed down** to keep only what is needed to benchmark DDO against `dd-suite`: the core solver plus the example problems used in the comparisons. **For any purpose other than reproducing the experiments in this repository, use the original repository** — it is the complete, maintained, and authoritative version.
>
> The original license is preserved in [`LICENSE.txt`](./LICENSE.txt) (© 2020 Xavier Gillard).

The comparisons are driven by the `main.rs` files under [`ddo/examples/`](./ddo/examples), one per problem:

- `ddo/examples/compare_knapsack/main.rs` — Knapsack
- `ddo/examples/compare_misp/main.rs` — Independent Set
- `ddo/examples/compare_scp/main.rs` — Set Cover
- `ddo/examples/compare_socknapsack/main.rs` — SOC Knapsack
- `ddo/examples/compare_scheduler/main.rs` — Scheduler

> ⚠️ The `main.rs` files inside `ddo/examples/compare_PROBLEM_NAME/` are used directly because a new executable has not been configured outside the examples directory.

The CSV results produced here are the ones stored in `../data_visualization/data/rust/`, used by `rust_plots.py` and `bounds_table.py` to compare Rust against the C++ and Python implementations.

---

## Requirements

- Rust (`cargo`)
- Bash (to run the `.sh` scripts)
- The `resources/` folder at the root of `ddo_simplificado/`, with the input files for each problem type (`knapsack/`, `misp/`, `scp/`, `socknapsack/`, `scheduler/`)

---

## How to Run

### 1. Install Rust and Cargo

To compile and run this project, you must have **Rust** and **Cargo** (Rust's package manager) installed.

You can install both with the following command:

```
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
```

Follow the on-screen instructions. This will install:

* rustc (the Rust compiler)
* cargo (a tool to build, test, and run Rust projects)

Verify the installation:

```
rustc --version
cargo --version
```

## 2. Navigate to the Project Root

All commands must be run from the root of the DDO project, i.e., inside the ddo_simplificado/ddo directory.

## 3. Run Examples Manually

To run an example manually, use the following command:

```
cargo run --example EXAMPLE_NAME -- INPUT_FILE OUTPUT_FILE COMPILE_TYPE MAX_WIDTH
```

* `EXAMPLE_NAME`: one of compare_knapsack, compare_misp, compare_scp, compare_socknapsack, or compare_scheduler
* `INPUT_FILE`: the name of the input file (without the full path)
* `OUTPUT_FILE`: the name of the output file (e.g., results.csv)
* `COMPILE_TYPE`: one of "Exact", "Restricted", or "Relaxed"
* `MAX_WIDTH`: only used with "Restricted" and "Relaxed". Ignored for "Exact" (but a dummy value like 99999 must still be passed)

### 📂 Input files (INPUT_FILE) must be located in:

* `../resources/knapsack/` for `compare_knapsack`
* `../resources/misp/` for `compare_misp`
* `../resources/scp/` for `compare_scp`
* `../resources/socknapsack/` for `compare_socknapsack`
* `../resources/scheduler/` for `compare_scheduler`

## 4. Run in Release Mode

For faster execution:

```
cargo run --release --example compare_knapsack -- INPUT_FILE OUTPUT_FILE COMPILE_TYPE MAX_WIDTH

```

## 5. Clean Previous Builds

To clean all previously compiled files:

```
cargo clean
```

## 📁 Available Scripts

There are Bash scripts (`.sh`) that automate the batch execution of experiments for each problem type:

* `run_knapsack.sh`
* `run_misp.sh`
* `run_scp.sh`
* `run_scheduler.sh`

### Example: run_knapsack.sh

This script:

* Iterates over all files in the ../resources/knapsack/ folder
* Runs the "Exact", "Restricted", and "Relaxed" variants
* Uses different MAX_WIDTH values for the last two
