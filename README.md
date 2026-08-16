# dd-suite

**`dd-suite`** is a research project focused on comparing decision diagram (DD) libraries across programming languages: the two reference implementations built here — **C++** and **Python** — plus a **Rust** baseline (a simplified copy of the external DDO library). The repository also contains the benchmark instances and the scripts that turn experimental results into plots and tables.

Supported optimization problems: **Knapsack**, **Independent Set**, **Set Cover**, **SOCKnapsack** (Second-Order Cone Knapsack) and **Sequencing**.

Each folder contains its own `README.md` with usage instructions, prerequisites, and important notes. Below is a general overview of the contents of each folder:

---

### `DD_c_plusplus`

The core C++ implementation of the decision diagram library. Includes CLI programs (`main`, `main_gurobi`, `main_cuts`) and standalone demos (`main_knapsack`, `main_socknapsack`). Build and run with the provided `makefile`.

---

### `DD_python`

The full Python implementation of the decision diagram library. Like the C++ version, this folder includes CLI handlers for running experiments from the terminal.

---

### `DataInstances`

Includes:

- Input files for executing problem instances in both Python and C++ (Knapsack, Independent Set, Set Cover, SOCKnapsack, Sequencing)
- Python scripts to generate new instances with custom parameters

This folder is useful for both testing and extending the suite with new benchmark problems.

---

### `ddo_simplificado`

A **simplified copy** of the **DDO** library by Xavier Gillard — the real and official repository is **[https://github.com/xgillard/ddo](https://github.com/xgillard/ddo)**, and that is the version you should use for anything beyond reproducing the experiments here.

This copy keeps only the core solver and the example problems (Knapsack, MISP, SCP, SOCKnapsack, Sequencing) needed to time Rust against `dd-suite`. It is not a fork we maintain, and it is not the DDO library itself: it is a trimmed-down snapshot kept for reproducibility, with the original license preserved in `ddo_simplificado/LICENSE.txt`.

---

### `data_visualization`

All data, plots, and tables derived from experimental runs are stored here, along with the scripts that build them from the raw CSV outputs of `DD_c_plusplus`, `DD_python`, and `ddo_simplificado`. The plots land in `plots/` (construction time C++ vs Python, Rust vs C++/Python, relax grouping vs relax priority) and the paper tables in `tables/` (Python vs C++ agreement, average construction-time ratio, SOC Knapsack cut summary).

---

Each folder is designed to work independently and includes a dedicated `README.md` to help you get started.

---

## Paper and Citation

We have a preprint associated with DD-suite available both in [arXiv](https://arxiv.org/abs/2608.10957) and [Optimization Online](https://optimization-online.org/2026/08/dd-suite-a-cross-platform-package-to-build-decision-diagrams-for-optimization-purposes/). Remember to cite our paper if you use DD-suite in your research project:

```bibtex
@article{blanco2026dd,
  title={DD-suite: A cross-platform package to build Decision Diagrams for optimization purposes},
  author={Blanco, Antonia F and Castro, Margarita and Toro Icarte, Rodrigo},
  journal={arXiv preprint arXiv:2608.10957},
  year={2026}
}
```
