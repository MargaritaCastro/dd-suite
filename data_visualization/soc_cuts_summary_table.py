import glob
import os
import re
import shutil
from datetime import datetime

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.font_manager import FontProperties


def archive_existing(folder):
    """Move the existing outputs of 'folder' into its 'deprecated/' subfolder."""
    deprecated = os.path.join(folder, "deprecated")
    os.makedirs(deprecated, exist_ok=True)
    for path in glob.glob(os.path.join(folder, "*")):
        if os.path.isfile(path):
            shutil.move(path, os.path.join(deprecated, os.path.basename(path)))

### Global parameters
# Subfolders holding the CSVs of each implementation
cpp_folder    = './data/cpp'
python_folder = './data/python'
output_folder = './tables/soc_cuts_summary_table'
archive_existing(output_folder)
file_timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")

TIMEOUT_SECONDS = 3600   # instances missing from the CSV = 60-minute timeout

CUT_MAP = {
    ("FlowCuts",      "False"): "FlowCuts",
    ("FlowCuts",      "True"):  "FlowCuts + Strengthening",
    ("JointFlowCuts", "False"): "JointFlowCuts",
    ("JointFlowCuts", "True"):  "JointFlowCuts + Strengthening",
    ("TargetCuts",    "False"): "TargetCuts",
}

VARIANT_ORDER = [
    "Gurobi",
    "FlowCuts", "FlowCuts + Strengthening",
    "JointFlowCuts", "JointFlowCuts + Strengthening",
    "TargetCuts",
]


def _file_datetime(path):
    """Extract the datetime from a name 'soc_*_DD_MM_YYYY_HH-MM-SS.csv'. The names
    cannot be sorted as strings: the day comes first, so the date must be parsed."""
    m = re.search(r"(\d{2}_\d{2}_\d{4}_\d{2}-\d{2}-\d{2})", os.path.basename(path))
    if not m:
        # No parseable date: fall back to mtime
        return datetime.fromtimestamp(os.path.getmtime(path))
    return datetime.strptime(m.group(1), "%d_%m_%Y_%H-%M-%S")

def load_latest(folder, pattern):
    files = glob.glob(os.path.join(folder, pattern))
    if not files:
        raise FileNotFoundError(f"Could not find '{pattern}' in {folder}")
    latest = max(files, key=_file_datetime)
    df = pd.read_csv(latest)
    df.columns = df.columns.str.strip()
    print(f"  {latest}  ({len(df)} rows)")
    return df

def compute_stats(df_variant, common_instances):
    """Metrics for one variant. All are averaged over `common_instances` (C++ ∩ Python)
    except # Solv, which counts every instance of this language with Gap == 0.
    Gap (%) averages only the unsolved instances, Time (s) only the solved ones."""
    # # Solve = instances solved to optimality, with a tolerance for numerical noise.
    if "Gap" in df_variant.columns:
        n_solv = int((df_variant["Gap"].abs() <= 1e-9).sum())
    else:
        n_solv = len(df_variant)

    sub = df_variant[df_variant["File_name"].isin(common_instances)]

    # Gap (%): mean ONLY over instances not solved to optimality (Gap != 0).
    # Time (s): mean ONLY over instances solved to optimality (Gap == 0).
    unsolved = sub[sub["Gap"].abs() > 1e-9]
    solved   = sub[sub["Gap"].abs() <= 1e-9]

    avg_gap  = unsolved["Gap"].mean() * 100
    avg_time = solved["Solution Time"].mean()

    has_nodecount = "NodeBB" in sub.columns
    avg_nodecount = sub["NodeBB"].mean() if has_nodecount else float("nan")

    has_cuts = "Num Cuts" in sub.columns
    avg_cuts = sub["Num Cuts"].mean() if has_cuts else float("nan")

    return {
        "# Solv":    n_solv,
        "Gap (%)":   avg_gap,
        "Time (s)":  avg_time,
        "NodeBB":    avg_nodecount,
        "# Cuts":    avg_cuts,
    }

def _variant_subset(df_cuts, cut_type, strengthening):
    return df_cuts[(df_cuts["Cut Type"] == cut_type) &
                   (df_cuts["CutStrengthening"].astype(str) == strengthening)]

def compute_common_instances(df_gurobi_cpp, df_cuts_cpp, df_gurobi_py, df_cuts_py):
    """
    Common set PER METHOD: for each variant, the instances present in the C++ CSV
    AND in the Python CSV for that same method.
    """
    common = {}
    common["Gurobi"] = set(df_gurobi_cpp["File_name"]) & set(df_gurobi_py["File_name"])
    for (cut_type, strengthening), name in CUT_MAP.items():
        cpp = set(_variant_subset(df_cuts_cpp, cut_type, strengthening)["File_name"])
        py  = set(_variant_subset(df_cuts_py,  cut_type, strengthening)["File_name"])
        common[name] = cpp & py
    return common

def build_results(df_gurobi, df_cuts, common_by_method):
    """Build the results DataFrame with one row per variant."""
    rows = []

    # Gurobi row
    rows.append({"Method": "Gurobi",
                 **compute_stats(df_gurobi, common_by_method["Gurobi"])})

    # Cut rows
    for (cut_type, strengthening), name in CUT_MAP.items():
        sub = _variant_subset(df_cuts, cut_type, strengthening)
        if sub.empty:
            print(f"  Warning: no data for {name}")
        rows.append({"Method": name,
                     **compute_stats(sub, common_by_method[name])})

    df_results = pd.DataFrame(rows).set_index("Method")
    df_results = df_results.reindex(VARIANT_ORDER).reset_index()
    return df_results


try:
    # ---------------------------------------------------------------------------
    # 1) Load the CSVs
    # ---------------------------------------------------------------------------
    print("Loading C++:")
    df_gurobi_cpp = load_latest(cpp_folder,    'soc_gurobi_*.csv')
    df_cuts_cpp   = load_latest(cpp_folder,    'soc_cuts_*.csv')

    print("Loading Python:")
    df_gurobi_py  = load_latest(python_folder, 'soc_gurobi_*.csv')
    df_cuts_py    = load_latest(python_folder, 'soc_cuts_*.csv')

    # ---------------------------------------------------------------------------
    # 2) Common set PER METHOD (instances present in C++ AND in Python)
    # ---------------------------------------------------------------------------
    common_by_method = compute_common_instances(
        df_gurobi_cpp, df_cuts_cpp, df_gurobi_py, df_cuts_py
    )
    print("\nCommon set per method (n instances averaged):")
    for name in VARIANT_ORDER:
        print(f"  {name:<32} n = {len(common_by_method.get(name, set()))}")

    # ---------------------------------------------------------------------------
    # 3) Compute the statistics for each implementation
    # ---------------------------------------------------------------------------
    df_cpp = build_results(df_gurobi_cpp, df_cuts_cpp, common_by_method)
    df_py  = build_results(df_gurobi_py,  df_cuts_py,  common_by_method)

    # ---------------------------------------------------------------------------
    # 4) Print the table to the console
    # ---------------------------------------------------------------------------
    print()
    print(f"{'Method':<30} {'C++ Solv':>9} {'C++ Gap':>9} {'C++ Time':>9} {'C++ Nodes':>11} {'C++ Cuts':>10}"
          f"  {'Py Solv':>8} {'Py Gap':>8} {'Py Time':>9} {'Py Nodes':>10} {'Py Cuts':>9}")
    print("-" * 125)
    for _, row_cpp in df_cpp.iterrows():
        name  = row_cpp["Method"]
        row_py = df_py[df_py["Method"] == name]
        py_vals = row_py.iloc[0] if not row_py.empty else None

        def v(df_row, col):
            val = df_row[col] if df_row is not None else float("nan")
            if pd.isna(val):      return "—"
            if col == "# Solv":   return str(int(val))
            if col == "Gap (%)":  return f"{val:.2f}%"
            if col == "Time (s)": return f"{val:.1f}"
            return f"{int(round(val)):,}"

        print(f"{name:<30}"
              f" {v(row_cpp, '# Solv'):>9} {v(row_cpp, 'Gap (%)'):>9}"
              f" {v(row_cpp, 'Time (s)'):>9} {v(row_cpp, 'NodeBB'):>11} {v(row_cpp, '# Cuts'):>10}"
              f"  {v(py_vals, '# Solv'):>8} {v(py_vals, 'Gap (%)'):>8}"
              f" {v(py_vals, 'Time (s)'):>9} {v(py_vals, 'NodeBB'):>10} {v(py_vals, '# Cuts'):>9}")
        if name == "Gurobi":
            print("-" * 125)
    print("-" * 125)
    print()

    # ---------------------------------------------------------------------------
    # 5) Renderizar figura
    # ---------------------------------------------------------------------------
    num_cols  = ["# Solv", "Gap (%)", "Time (s)", "NodeBB", "# Cuts"]
    languages = [("C++", df_cpp), ("Python", df_py)]

    # Best value per column and language (for the bold face)
    best = {}
    for lang_name, df_lang in languages:
        best[lang_name] = {}
        for col in num_cols:
            vals = pd.to_numeric(df_lang[col], errors="coerce")
            best[lang_name][col] = vals.min() if col != "# Solv" else vals.max()

    def fmt(val, col):
        if pd.isna(val):        return "—"
        if col == "# Solv":     return str(int(val))
        if col == "Gap (%)":    return f"{val:.2f}%"
        if col == "Time (s)":   return f"{val:.1f}"
        if col == "NodeBB":     return f"{int(round(val)):,}"
        if col == "# Cuts":     return f"{int(round(val)):,}"
        return str(val)

    n_rows    = len(df_cpp)
    row_height = 0.35
    header_h1  = 0.40   # height of the "C++ / Python" row
    header_h2  = 0.40   # height of the column-name row
    total_h    = header_h1 + header_h2 + n_rows * row_height

    # Column widths: method | 5 C++ cols | sep | 5 Python cols
    method_w = 2.8
    data_w   = [0.8, 0.85, 0.9, 1.0, 0.85]
    sep_w    = 0.15
    col_widths = [method_w] + data_w + [sep_w] + data_w
    total_w    = sum(col_widths)

    fig, ax = plt.subplots(figsize=(total_w + 0.4, total_h + 0.5))
    ax.axis("off")

    col_left  = [sum(col_widths[:i]) / total_w for i in range(len(col_widths))]
    col_right = [col_left[i] + col_widths[i] / total_w for i in range(len(col_widths))]

    # Column indices in the layout: 0=method, 1-5=cpp, 6=sep, 7-11=python
    cpp_col_idx = [1, 2, 3, 4, 5]
    py_col_idx  = [7, 8, 9, 10, 11]

    h1_y  = (total_h - header_h1 / 2)             / total_h
    h2_y  = (total_h - header_h1 - header_h2 / 2) / total_h
    row_y = [(total_h - header_h1 - header_h2 - r * row_height - row_height / 2) / total_h
             for r in range(n_rows)]

    font_normal  = FontProperties(family="serif", size=8.5)
    font_bold    = FontProperties(family="serif", size=8.5, weight="bold")
    font_header  = FontProperties(family="serif", size=8.5, weight="bold")

    def write(x, y, text, ha, font):
        ax.text(x, y, text, ha=ha, va="center",
                fontproperties=font, transform=ax.transAxes)

    # ── Header row 1: "C++" and "Python" centered over their groups ──
    cpp_group_cx = (col_left[1] + col_right[5]) / 2
    py_group_cx  = (col_left[7] + col_right[11]) / 2
    write(cpp_group_cx, h1_y, "C++",    "center", font_header)
    write(py_group_cx,  h1_y, "Python", "center", font_header)

    # ── Header row 2: column names ──
    write(col_left[0], h2_y, "Method", "left", font_header)
    col_labels = ["# Solv", "Gap (%)", "Time (s)", "NodeBB", "# Cuts"]
    for j, label in enumerate(col_labels):
        write(col_right[cpp_col_idx[j]], h2_y, label, "right", font_header)
        write(col_right[py_col_idx[j]],  h2_y, label, "right", font_header)

    # ── Data rows ──
    for r, (_, row_cpp) in enumerate(df_cpp.iterrows()):
        name   = row_cpp["Method"]
        row_py = df_py[df_py["Method"] == name]
        py_row = row_py.iloc[0] if not row_py.empty else None
        yc     = row_y[r]

        write(col_left[0], yc, name, "left", font_normal)

        for j, col in enumerate(num_cols):
            # C++
            val_cpp = row_cpp[col]
            numeric_cpp = pd.to_numeric(val_cpp, errors="coerce")
            is_best_cpp = (not np.isnan(numeric_cpp)
                           and abs(numeric_cpp - best["C++"][col]) < 1e-9)
            write(col_right[cpp_col_idx[j]], yc, fmt(val_cpp, col), "right",
                  font_bold if is_best_cpp else font_normal)

            # Python
            val_py = py_row[col] if py_row is not None else float("nan")
            numeric_py = pd.to_numeric(val_py, errors="coerce")
            is_best_py = (not np.isnan(numeric_py)
                          and abs(numeric_py - best["Python"][col]) < 1e-9)
            write(col_right[py_col_idx[j]], yc, fmt(val_py, col), "right",
                  font_bold if is_best_py else font_normal)

    # ── Horizontal lines ──
    def hline(y, lw, x0=0, x1=1):
        ax.plot([x0, x1], [y, y], color="black", linewidth=lw,
                transform=ax.transAxes, clip_on=False)

    hline(1.0, 1.5)
    hline((total_h - header_h1)             / total_h, 0.6)   # bajo "C++ / Python"
    hline((total_h - header_h1 - header_h2) / total_h, 1.0)   # below the column names
    hline((row_y[0] - row_height / 2 / total_h),       0.5)   # Gurobi / cuts divider
    hline(0.0, 1.5)

    # Vertical separator between groups
    sep_x = (col_left[6] + col_right[6]) / 2
    ax.plot([sep_x, sep_x], [0, (total_h - header_h1) / total_h],
            color="black", linewidth=0.6,
            transform=ax.transAxes, clip_on=False)

    plt.tight_layout(pad=0.5)
    os.makedirs(output_folder, exist_ok=True)
    output_path = os.path.join(output_folder, f"soc_cuts_summary_{file_timestamp}.png")
    plt.savefig(output_path, dpi=180, bbox_inches="tight")
    print(f"Table saved to: {output_path}")

except Exception as e:
    print(f"Error: {e}")
