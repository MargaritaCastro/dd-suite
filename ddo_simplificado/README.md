# Antonia Blanco — Experiments with DDO in Rust 

This project is based on code created by **Xavier Gillard** in the repository [https://github.com/xgillard/ddo](https://github.com/xgillard/ddo) to compare different types of decision diagram (DDO) compilations. The main files used to perform these comparisons are located in the [`ddo/examples`](./ddo/examples) folder, specifically in:

- `ddo/examples/compare_knapsack/main.rs` — Knapsack
- `ddo/examples/compare_misp/main.rs` — Independent Set
- `ddo/examples/compare_scp/main.rs` — Set Cover
- `ddo/examples/compare_socknapsack/main.rs` — SOC Knapsack
- `ddo/examples/compare_sequencing/main.rs` — Sequencing

> ⚠️ The `main.rs` files inside `/examples/compare_PROBLEM_NAME/` are used directly because a new executable has not been configured outside the examples directory.

---

## Requirements

- Rust (`cargo`)
- Bash (to run the `.sh` scripts)
- A `resources/` folder with input files for each problem type

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

* `EXAMPLE_NAME`: one of compare_knapsack, compare_misp, compare_scp, compare_socknapsack, or compare_sequencing
* `INPUT_FILE`: the name of the input file (without the full path)
* `OUTPUT_FILE`: the name of the output file (e.g., results.csv)
* `COMPILE_TYPE`: one of "Exact", "Restricted", or "Relaxed"
* `MAX_WIDTH`: only used with "Restricted" and "Relaxed". Ignored for "Exact" (but a dummy value like 99999 must still be passed)

### 📂 Input files (INPUT_FILE) must be located in:

* `../resources/knapsack/` for `compare_knapsack`
* `../resources/misp/` for `compare_misp`
* `../resources/scp/` for `compare_scp`
* `../resources/socknapsack/` for `compare_socknapsack`
* `../resources/sequencing/` for `compare_sequencing`

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

Bash scripts (`.sh`) that automate the batch execution of experiments. There is one per problem except SOC Knapsack, which is only run from the cluster scripts:

* `run_knapsack.sh`
* `run_misp.sh`
* `run_scp.sh`
* `run_sequencing.sh`

### Example: run_knapsack.sh

This script:

* Iterates over all files in the ../resources/knapsack/ folder
* Runs the "Exact", "Restricted", and "Relaxed" variants
* Uses different MAX_WIDTH values for the last two
