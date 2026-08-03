
import glob
import os
import re
import shutil
from datetime import datetime

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.backends.backend_pdf as pdf_backend
from matplotlib.font_manager import FontProperties


def archive_existing(folder):
    """Create 'folder' and its 'deprecated/' and move any existing outputs there
    before generating new ones, so old versions do not pile up in the root."""
    deprecated = os.path.join(folder, "deprecated")
    os.makedirs(deprecated, exist_ok=True)
    for path in glob.glob(os.path.join(folder, "*")):
        if os.path.isfile(path):
            shutil.move(path, os.path.join(deprecated, os.path.basename(path)))


BASE     = os.path.dirname(os.path.abspath(__file__))
CPP_DIR  = os.path.join(BASE, "data", "cpp")
PY_DIR   = os.path.join(BASE, "data", "python")
OUT_DIR  = os.path.join(BASE, "tables", "python_cpp_agreement_table")
OUT_PATH = os.path.join(OUT_DIR, f"python_cpp_agreement_{datetime.now().strftime('%Y%m%d_%H%M%S')}.pdf")

archive_existing(OUT_DIR)


def _file_datetime(path):
    """Extract the datetime from a '..._DD_MM_YYYY_HH-MM-SS.csv' name.

    The names cannot be sorted as strings: the day comes first, so '27_05_2026'
    (27 May) would sort after '01_06_2026' (1 Jun). The real date must be parsed.
    """
    m = re.search(r"(\d{2}_\d{2}_\d{4}_\d{2}-\d{2}-\d{2})", os.path.basename(path))
    if not m:
        return datetime.fromtimestamp(os.path.getmtime(path))
    return datetime.strptime(m.group(1), "%d_%m_%Y_%H-%M-%S")


def load_latest(folder, pattern, exclude=None):
    files = glob.glob(os.path.join(folder, pattern))
    if exclude:
        files = [f for f in files
                 if not any(e in os.path.basename(f).lower() for e in exclude)]
    if not files:
        raise FileNotFoundError(f"Could not find '{pattern}' in {folder}")
    latest = max(files, key=_file_datetime)
    df = pd.read_csv(latest)
    df.columns = df.columns.str.strip()
    for col in df.columns:
        if df[col].dtype == object:
            df[col] = df[col].astype(str).str.replace("\x00", "", regex=False).str.strip()
    print(f"  {latest}  ({len(df)} rows)")
    return df, os.path.basename(latest)


# -- Load -----------------------------------------------------------------------
print("Loading DD Python:")
df_py, src_py = load_latest(PY_DIR, "results_python_*.csv", exclude=["grouping"])

print("Loading DD C++:")
df_cpp, src_cpp = load_latest(CPP_DIR, "results_cpp_*.csv", exclude=["grouping"])

# Legacy alias: CSVs from before the problem was renamed carry "Scheduler".
# Normalised BEFORE the merge, which uses Problem type as part of the key.
for _df in (df_py, df_cpp):
    _df["Problem type"] = _df["Problem type"].replace({"Scheduler": "Sequencing"})


# -- Configuration key ----------------------------------------------------------
# 'Is Reduced' and 'Sort' are lowercased because Python writes True/False while
# C++ writes true/false: without this no row would ever match.
KEY  = ["File_name", "Problem type", "DD Type", "Max Width",
        "Is Reduced", "Objective Type", "Sort"]
BOOL_COLS = ["Is Reduced", "Sort"]

# Metrics to compare: (CSV column, label, relative tolerance)
# Node and arc counts are integers and must match exactly. The objective value is
# compared with a tolerance because C++ writes it in floating-point form
# ("253.000000") while Python writes an integer ("253").
METRICS = [
    ("Optimization Value", "Objective value", 1e-4),
    ("Number Node",        "Node count",      0.0),
    ("Number Arcs",        "Arc count",       0.0),
]

PROBLEM_ORDER = ["Knapsack", "Independent Set", "Set Cover", "SOC Knapsack", "Sequencing"]
DD_ORDER      = ["Exact", "Restricted", "RelaxPriority", "RelaxGrouping"]


def normalize(df):
    out = df.copy()
    for c in BOOL_COLS:
        out[c] = out[c].astype(str).str.strip().str.lower()
    for c in KEY:
        out[c] = out[c].astype(str).str.strip()
    return out


df_py  = normalize(df_py)
df_cpp = normalize(df_cpp)

METRIC_COLS = [m[0] for m in METRICS]

# If a CSV repeats the same key we keep the last occurrence: merging on duplicated
# keys would produce a cartesian product and inflate the totals.
dup_py  = int(df_py.duplicated(subset=KEY).sum())
dup_cpp = int(df_cpp.duplicated(subset=KEY).sum())
if dup_py or dup_cpp:
    print(f"  Warning: duplicated keys (Py={dup_py}, C++={dup_cpp}); keeping the last one.")
df_py  = df_py.drop_duplicates(subset=KEY, keep="last")
df_cpp = df_cpp.drop_duplicates(subset=KEY, keep="last")

merged = df_py[KEY + METRIC_COLS].merge(
    df_cpp[KEY + METRIC_COLS], on=KEY, how="inner", suffixes=("_py", "_cpp"))

only_py  = len(df_py)  - len(merged)
only_cpp = len(df_cpp) - len(merged)
print(f"\nCommon configurations: {len(merged)}  "
      f"(Python only: {only_py}, C++ only: {only_cpp})")


def eq_series(col, tol):
    """Boolean agreement series for one metric.

    'na' shows up in Reduce Time and may show up in other columns; to_numeric turns
    it into NaN and there is nothing to compare there, so the row counts neither as
    an agreement nor as a mismatch (it stays out of the denominator).
    """
    a = pd.to_numeric(merged[f"{col}_py"],  errors="coerce")
    b = pd.to_numeric(merged[f"{col}_cpp"], errors="coerce")
    valid = a.notna() & b.notna()
    if tol == 0:
        same = a == b
    else:
        same = (a - b).abs() <= tol * (1 + b.abs())
    return valid, (same & valid)


for col, _, tol in METRICS:
    valid, same = eq_series(col, tol)
    merged[f"_valid_{col}"] = valid
    merged[f"_same_{col}"]  = same


# -- Problem x DD type matrices -------------------------------------------------
PROBLEMS = [p for p in PROBLEM_ORDER if p in merged["Problem type"].values]
DDTYPES  = [d for d in DD_ORDER      if d in merged["DD Type"].values]


def build_matrix(col):
    """Return dict[(problem, dd_type)] = (matching, comparable)."""
    cells = {}
    for p in PROBLEMS:
        for d in DDTYPES:
            sub = merged[(merged["Problem type"] == p) & (merged["DD Type"] == d)]
            total = int(sub[f"_valid_{col}"].sum())
            same  = int(sub[f"_same_{col}"].sum())
            cells[(p, d)] = (same, total)
    return cells


# -- Rendering ------------------------------------------------------------------
FN  = FontProperties(family="monospace", size=7.5)
FNB = FontProperties(family="monospace", size=7.5, weight="bold")
FH  = FontProperties(family="monospace", size=8,   weight="bold")

COL_W_LBL = 1.9      # width of the label column (problems)
COL_W     = 1.35     # width of each DD type column
ROW_H     = 0.34
HDR_H     = 0.42


def cell_color(same, total):
    """Green when everything agrees, shading to red as agreement drops."""
    if total == 0:
        return "white"
    frac = same / total
    if frac == 1.0:
        return "#c8e6c9"
    if frac >= 0.75:
        return "#fff4c2"
    if frac >= 0.40:
        return "#ffd9b3"
    return "#ffcdd2"


def fmt_cell(same, total):
    if total == 0:
        return "-"
    return f"{same}/{total}"


def render_matrix(ax, cells, title):
    n_rows = len(PROBLEMS) + 1                          # + Total row
    total_w = COL_W_LBL + COL_W * (len(DDTYPES) + 1)    # + Total column
    total_h = HDR_H + n_rows * ROW_H

    ax.axis("off")
    ax.set_xlim(0, total_w)
    ax.set_ylim(0, total_h)
    ax.set_title(title, loc="left", fontproperties=FH, pad=6)

    headers = DDTYPES + ["Total"]

    def col_x(j):
        """Left edge of column j (0-based over headers)."""
        return COL_W_LBL + j * COL_W

    def row_y(i):
        """Vertical centre of row i (0-based, 0 = first problem)."""
        return total_h - HDR_H - (i + 0.5) * ROW_H

    # Header
    for j, h in enumerate(headers):
        ax.text(col_x(j) + COL_W / 2, total_h - HDR_H / 2, h,
                ha="center", va="center", fontproperties=FH)

    col_tot = {d: [0, 0] for d in DDTYPES}
    grand   = [0, 0]

    for i, p in enumerate(PROBLEMS):
        y = row_y(i)
        ax.text(0.04, y, p, ha="left", va="center", fontproperties=FNB)

        row_tot = [0, 0]
        for j, d in enumerate(DDTYPES):
            same, total = cells[(p, d)]
            row_tot[0] += same;  row_tot[1] += total
            col_tot[d][0] += same;  col_tot[d][1] += total
            ax.add_patch(plt.Rectangle((col_x(j), y - ROW_H / 2), COL_W, ROW_H,
                                       color=cell_color(same, total), zorder=0))
            ax.text(col_x(j) + COL_W / 2, y, fmt_cell(same, total),
                    ha="center", va="center",
                    fontproperties=FNB if (total and same == total) else FN)

        grand[0] += row_tot[0];  grand[1] += row_tot[1]
        j = len(DDTYPES)
        ax.add_patch(plt.Rectangle((col_x(j), y - ROW_H / 2), COL_W, ROW_H,
                                   color=cell_color(*row_tot), zorder=0))
        ax.text(col_x(j) + COL_W / 2, y, fmt_cell(*row_tot),
                ha="center", va="center", fontproperties=FNB)

    # Totals row, per DD type
    y = row_y(len(PROBLEMS))
    ax.text(0.04, y, "Total", ha="left", va="center", fontproperties=FH)
    for j, d in enumerate(DDTYPES):
        same, total = col_tot[d]
        ax.add_patch(plt.Rectangle((col_x(j), y - ROW_H / 2), COL_W, ROW_H,
                                   color=cell_color(same, total), zorder=0))
        ax.text(col_x(j) + COL_W / 2, y, fmt_cell(same, total),
                ha="center", va="center", fontproperties=FH)
    j = len(DDTYPES)
    ax.add_patch(plt.Rectangle((col_x(j), y - ROW_H / 2), COL_W, ROW_H,
                               color=cell_color(*grand), zorder=0))
    ax.text(col_x(j) + COL_W / 2, y, fmt_cell(*grand),
            ha="center", va="center", fontproperties=FH)

    # Rules: below the header, above the Total row, and around the Total column
    ax.plot([0, total_w], [total_h - HDR_H] * 2, color="black", lw=0.8)
    ax.plot([0, total_w], [row_y(len(PROBLEMS)) + ROW_H / 2] * 2, color="black", lw=0.8)
    ax.plot([col_x(len(DDTYPES))] * 2, [0, total_h - HDR_H], color="black", lw=0.8)
    ax.plot([COL_W_LBL] * 2, [0, total_h], color="black", lw=0.8)

    return grand


print(f"\nWriting PDF -> {OUT_PATH}")

n_rows_fig = len(PROBLEMS) + 1
fig_h = len(METRICS) * (HDR_H + n_rows_fig * ROW_H + 0.75) + 0.9
fig_w = COL_W_LBL + COL_W * (len(DDTYPES) + 1) + 0.6

fig, axes = plt.subplots(len(METRICS), 1, figsize=(fig_w, fig_h))
if len(METRICS) == 1:
    axes = [axes]

summary = {}
for ax, (col, label, _) in zip(axes, METRICS):
    grand = render_matrix(ax, build_matrix(col), f"{label}  -  Python vs C++ agreement")
    summary[label] = grand

pct = lambda s, t: (100 * s / t) if t else 0
fig.suptitle(
    "Python vs C++ agreement by problem and DD type\n"
    f"Python: {src_py}   |   C++: {src_cpp}\n"
    f"{len(merged)} common configurations "
    f"(Python only: {only_py}, C++ only: {only_cpp}). "
    "Each cell: matching / comparable.",
    fontproperties=FontProperties(family="monospace", size=8), y=0.995)

plt.tight_layout(rect=[0, 0, 1, 0.955])
with pdf_backend.PdfPages(OUT_PATH) as pdf:
    pdf.savefig(fig, bbox_inches="tight")
plt.close(fig)

print("\nSummary:")
for label, (s, t) in summary.items():
    print(f"  {label:<16} {s}/{t}  ({pct(s, t):.1f}%)")
print("\nDone!")
