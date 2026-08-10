
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
OUT_DIR  = os.path.join(BASE, "tables", "bounds_validity_table")
OUT_PATH = os.path.join(OUT_DIR, f"bounds_validity_{datetime.now().strftime('%Y%m%d_%H%M%S')}.pdf")

archive_existing(OUT_DIR)


def _file_datetime(path):
    """Datetime of a '..._DD_MM_YYYY_HH-MM-SS.csv' name: the day comes first, so
    the names cannot be sorted as strings."""
    m = re.search(r"(\d{2}_\d{2}_\d{4}_\d{2}-\d{2}-\d{2})", os.path.basename(path))
    if not m:
        return datetime.fromtimestamp(os.path.getmtime(path))
    return datetime.strptime(m.group(1), "%d_%m_%Y_%H-%M-%S")


def load_latest(folder, pattern, exclude=None, optional=False):
    files = glob.glob(os.path.join(folder, pattern))
    if exclude:
        files = [f for f in files
                 if not any(e in os.path.basename(f).lower() for e in exclude)]
    if not files:
        if optional:
            print(f"  (no '{pattern}' in {folder}; skipped)")
            return None, None
        raise FileNotFoundError(f"Could not find '{pattern}' in {folder}")
    latest = max(files, key=_file_datetime)
    df = pd.read_csv(latest)
    df.columns = df.columns.str.strip()
    for col in df.columns:
        if df[col].dtype == object:
            df[col] = df[col].astype(str).str.replace("\x00", "", regex=False).str.strip()
    df.attrs["source_datetime"] = _file_datetime(latest)   # used by combine_exact
    print(f"  {latest}  ({len(df)} rows)")
    return df, os.path.basename(latest)


# ── Configuration ──────────────────────────────────────────────────────────────
# Only C++: python_cpp_agreement_table.py already shows both build the same
# diagrams, and C++ covers more instances.

# The values are written as floats, so an exact comparison would flag rounding
# noise as a violation.
REL_TOL = 1e-6

# Restricted gives a feasible (primal) solution, relaxed a bound over the whole
# feasible set.
DD_ROLE = {
    "Restricted":    "restricted",
    "RelaxPriority": "relaxed",
    "RelaxGrouping": "relaxed",
    "Relaxed":       "relaxed",   # legacy name used by CSVs predating the split
}

PROBLEM_ORDER = ["Knapsack", "Independent Set", "Set Cover", "SOC Knapsack", "Sequencing"]
DD_ORDER      = ["Restricted", "RelaxPriority", "RelaxGrouping"]

# SOC Knapsack: its DD models a single constraint, so Gurobi's optimum of the full
# problem is not what it bounds; its own exact DD still is.
GUROBI_EXCLUDED_PROBLEMS = ["SOC Knapsack"]


# ── Loading ────────────────────────────────────────────────────────────────────
print("Loading DD C++ (general):")
df_cpp, src_cpp = load_latest(CPP_DIR, "results_cpp_*.csv", exclude=["grouping"])

print("Loading DD C++ (dedicated exact):")
df_cpp_exact_ded, src_exact = load_latest(CPP_DIR, "results_exact_cpp_*.csv", optional=True)

print("Loading Gurobi C++:")
df_gur, src_gur = load_latest(CPP_DIR, "results_gurobi_cpp_*.csv")

ALL_DFS = tuple(d for d in (df_cpp, df_cpp_exact_ded, df_gur) if d is not None)

# Legacy alias, normalised before the merges (which key on Problem type).
for _df in ALL_DFS:
    _df["Problem type"] = _df["Problem type"].replace({"Scheduler": "Sequencing"})
    _df["File_name"] = _df["File_name"].astype(str).str.replace(
        r"^scheduler_", "sequencing_", regex=True)

KEY = ["File_name", "Problem type"]


# ── Reference optimum ──────────────────────────────────────────────────────────
def filter_exact(df):
    """Exact DDs WITHOUT reduction: reducing can alter the value and contaminate
    the reference."""
    dd  = df["DD Type"].astype(str).str.strip()
    red = df["Is Reduced"].astype(str).str.strip().str.lower()
    out = df[(dd == "Exact") & (red == "false")][KEY + ["Optimization Value"]].copy()
    out["Optimization Value"] = pd.to_numeric(out["Optimization Value"], errors="coerce")
    return out.dropna(subset=["Optimization Value"])


def combine_exact(df_general, *dfs_dedicated):
    """Union of the exact (non-reduced) values from the general file and the
    dedicated ones. On duplicates the most recent run wins, so that an old
    dedicated CSV never overrides a newer general one."""
    dfs = tuple(d for d in (df_general, *dfs_dedicated) if d is not None)
    order = sorted(range(len(dfs)),
                   key=lambda i: dfs[i].attrs.get("source_datetime", datetime.min))
    combined = pd.concat([filter_exact(dfs[i]) for i in order], ignore_index=True)
    return combined.drop_duplicates(subset=KEY, keep="last")


ref_exact = combine_exact(df_cpp, df_cpp_exact_ded).rename(
    columns={"Optimization Value": "Exact_Opt"})

# Only a closed gap certifies optimality; anything else is just an incumbent.
gur = df_gur.copy()
gur["Gap"]           = pd.to_numeric(gur["Gap"], errors="coerce")
gur["Best solution"] = pd.to_numeric(gur["Best solution"], errors="coerce")
ref_gurobi = (gur[(gur["Gap"].abs() <= 1e-9) & gur["Best solution"].notna() &
                  ~gur["Problem type"].isin(GUROBI_EXCLUDED_PROBLEMS)]
              [KEY + ["Best solution"]]
              .rename(columns={"Best solution": "Gurobi_Opt"})
              .drop_duplicates(subset=KEY, keep="last"))

ref = ref_exact.merge(ref_gurobi, on=KEY, how="outer")
# The exact DD wins: it is the optimum of the model the other diagrams approximate.
ref["Optimum"]    = ref["Exact_Opt"].where(ref["Exact_Opt"].notna(), ref["Gurobi_Opt"])
ref["Ref_source"] = np.where(ref["Exact_Opt"].notna(), "Exact DD", "Gurobi gap 0")
ref = ref.dropna(subset=["Optimum"])

n_exact  = int((ref["Ref_source"] == "Exact DD").sum())
n_gurobi = int((ref["Ref_source"] == "Gurobi gap 0").sum())
print(f"\nInstances with a known optimum: {len(ref)}  "
      f"(exact DD: {n_exact}, Gurobi gap 0 only: {n_gurobi})")

# A disagreement makes the reference itself suspect: report it before going on.
both = ref[ref["Exact_Opt"].notna() & ref["Gurobi_Opt"].notna()]
mismatch = both[(both["Exact_Opt"] - both["Gurobi_Opt"]).abs()
                > 1e-4 * (1 + both["Gurobi_Opt"].abs())]
if len(mismatch):
    print(f"  Warning: exact DD and Gurobi disagree on {len(mismatch)} instances "
          f"(exact DD used); e.g. "
          + ", ".join(f"{r['File_name']} ({r['Exact_Opt']:g} vs {r['Gurobi_Opt']:g})"
                      for _, r in mismatch.head(3).iterrows()))


# ── Bound check ────────────────────────────────────────────────────────────────
checks = df_cpp[df_cpp["DD Type"].isin(DD_ROLE)].copy()
checks["Optimization Value"] = pd.to_numeric(checks["Optimization Value"], errors="coerce")
checks["Max Width"]          = pd.to_numeric(checks["Max Width"], errors="coerce")
checks["Objective Type"]     = checks["Objective Type"].astype(str).str.strip().str.lower()
checks["Is Reduced"]         = checks["Is Reduced"].astype(str).str.strip().str.lower()
checks["Sort"]               = checks["Sort"].astype(str).str.strip().str.lower()

n_before = len(checks)
checks = checks.merge(ref[KEY + ["Optimum", "Ref_source"]], on=KEY, how="inner")
checks = checks.dropna(subset=["Optimization Value", "Optimum"])
print(f"Restricted/Relaxed rows: {n_before} in the CSV, "
      f"{len(checks)} checkable (the rest has no known optimum)")

checks["Role"] = checks["DD Type"].map(DD_ROLE)


def bound_holds(row):
    """Restricted is never better than the optimum, relaxed never worse; 'better'
    flips with the objective sense."""
    opt, val = row["Optimum"], row["Optimization Value"]
    tol = REL_TOL * (1 + abs(opt))
    if row["Role"] == "restricted":
        return val <= opt + tol if row["Objective Type"] == "max" else val >= opt - tol
    return val >= opt - tol if row["Objective Type"] == "max" else val <= opt + tol


def violation_amount(row):
    """Signed distance past the bound (0 when it holds), for ranking the report."""
    opt, val = row["Optimum"], row["Optimization Value"]
    if row["Role"] == "restricted":
        excess = (val - opt) if row["Objective Type"] == "max" else (opt - val)
    else:
        excess = (opt - val) if row["Objective Type"] == "max" else (val - opt)
    return max(excess, 0.0)


checks["_ok"]        = checks.apply(bound_holds, axis=1)
checks["_violation"] = checks.apply(violation_amount, axis=1)
checks["_rel_viol"]  = checks["_violation"] / (1 + checks["Optimum"].abs())

violations = checks[~checks["_ok"]].sort_values(
    ["Problem type", "_rel_viol"], ascending=[True, False])

PROBLEMS = [p for p in PROBLEM_ORDER if p in checks["Problem type"].values]
DDTYPES  = [d for d in DD_ORDER      if d in checks["DD Type"].values]
WIDTHS   = sorted(int(w) for w in checks["Max Width"].dropna().unique())


def build_matrix(cols, col_field):
    """Return dict[(problem, col)] = (holding, checked)."""
    cells = {}
    for p in PROBLEMS:
        for c in cols:
            sub = checks[(checks["Problem type"] == p) & (checks[col_field] == c)]
            cells[(p, c)] = (int(sub["_ok"].sum()), len(sub))
    return cells


# ── Rendering ──────────────────────────────────────────────────────────────────
FN  = FontProperties(family="monospace", size=7.5)
FNB = FontProperties(family="monospace", size=7.5, weight="bold")
FH  = FontProperties(family="monospace", size=8,   weight="bold")

COL_W_LBL = 1.9      # width of the label column (problems)
COL_W     = 1.35     # width of each data column
ROW_H     = 0.34
HDR_H     = 0.42


def cell_color(ok, total):
    """Green when every bound holds, shading to red as violations appear."""
    if total == 0:
        return "white"
    frac = ok / total
    if frac == 1.0:
        return "#c8e6c9"
    if frac >= 0.99:
        return "#fff4c2"
    if frac >= 0.90:
        return "#ffd9b3"
    return "#ffcdd2"


def fmt_cell(ok, total):
    if total == 0:
        return "-"
    return f"{ok}/{total}"


def render_matrix(ax, cells, headers, title):
    n_rows  = len(PROBLEMS) + 1                          # + Total row
    total_w = COL_W_LBL + COL_W * (len(headers) + 1)     # + Total column
    total_h = HDR_H + n_rows * ROW_H

    ax.axis("off")
    ax.set_xlim(0, total_w)
    ax.set_ylim(0, total_h)
    ax.set_title(title, loc="left", fontproperties=FH, pad=6)

    head_lbls = [str(h) for h in headers] + ["Total"]

    def col_x(j):
        """Left edge of column j (0-based over head_lbls)."""
        return COL_W_LBL + j * COL_W

    def row_y(i):
        """Vertical centre of row i (0-based, 0 = first problem)."""
        return total_h - HDR_H - (i + 0.5) * ROW_H

    for j, h in enumerate(head_lbls):
        ax.text(col_x(j) + COL_W / 2, total_h - HDR_H / 2, h,
                ha="center", va="center", fontproperties=FH)

    col_tot = {c: [0, 0] for c in headers}
    grand   = [0, 0]

    for i, p in enumerate(PROBLEMS):
        y = row_y(i)
        ax.text(0.04, y, p, ha="left", va="center", fontproperties=FNB)

        row_tot = [0, 0]
        for j, c in enumerate(headers):
            ok, total = cells[(p, c)]
            row_tot[0] += ok;  row_tot[1] += total
            col_tot[c][0] += ok;  col_tot[c][1] += total
            ax.add_patch(plt.Rectangle((col_x(j), y - ROW_H / 2), COL_W, ROW_H,
                                       color=cell_color(ok, total), zorder=0))
            ax.text(col_x(j) + COL_W / 2, y, fmt_cell(ok, total),
                    ha="center", va="center",
                    fontproperties=FNB if (total and ok == total) else FN)

        grand[0] += row_tot[0];  grand[1] += row_tot[1]
        j = len(headers)
        ax.add_patch(plt.Rectangle((col_x(j), y - ROW_H / 2), COL_W, ROW_H,
                                   color=cell_color(*row_tot), zorder=0))
        ax.text(col_x(j) + COL_W / 2, y, fmt_cell(*row_tot),
                ha="center", va="center", fontproperties=FNB)

    # Totals row, per column
    y = row_y(len(PROBLEMS))
    ax.text(0.04, y, "Total", ha="left", va="center", fontproperties=FH)
    for j, c in enumerate(headers):
        ok, total = col_tot[c]
        ax.add_patch(plt.Rectangle((col_x(j), y - ROW_H / 2), COL_W, ROW_H,
                                   color=cell_color(ok, total), zorder=0))
        ax.text(col_x(j) + COL_W / 2, y, fmt_cell(ok, total),
                ha="center", va="center", fontproperties=FH)
    j = len(headers)
    ax.add_patch(plt.Rectangle((col_x(j), y - ROW_H / 2), COL_W, ROW_H,
                               color=cell_color(*grand), zorder=0))
    ax.text(col_x(j) + COL_W / 2, y, fmt_cell(*grand),
            ha="center", va="center", fontproperties=FH)

    # Rules: below the header, above the Total row, and around the Total column
    ax.plot([0, total_w], [total_h - HDR_H] * 2, color="black", lw=0.8)
    ax.plot([0, total_w], [row_y(len(PROBLEMS)) + ROW_H / 2] * 2, color="black", lw=0.8)
    ax.plot([col_x(len(headers))] * 2, [0, total_h - HDR_H], color="black", lw=0.8)
    ax.plot([COL_W_LBL] * 2, [0, total_h], color="black", lw=0.8)

    return grand


# ── Violation report ───────────────────────────────────────────────────────────
def fmt_val(v):
    if pd.isna(v):
        return "—"
    try:
        if float(v) == int(float(v)):
            return f"{int(float(v)):,}"
    except (ValueError, OverflowError):
        pass
    return f"{float(v):,.4f}"


def violation_case(row):
    """Human-readable statement of the inequality that was broken."""
    if row["Role"] == "restricted":
        rel = "above" if row["Objective Type"] == "max" else "below"
        return f"Restricted ({row['Objective Type']}) {rel} the optimum"
    rel = "below" if row["Objective Type"] == "max" else "above"
    return f"{row['DD Type']} ({row['Objective Type']}) {rel} the optimum"


def violation_lines():
    """One text line per violating configuration, for the console and the PDF."""
    lines = []
    for _, r in violations.iterrows():
        lines.append(
            f"{r['File_name']}  [{r['Problem type']}]  "
            f"{r['DD Type']} w={int(r['Max Width'])} reduced={r['Is Reduced']} "
            f"sort={r['Sort']}  ->  value {fmt_val(r['Optimization Value'])} "
            f"vs optimum {fmt_val(r['Optimum'])} ({r['Ref_source']})  "
            f"| {violation_case(r)} by {fmt_val(r['_violation'])} "
            f"({100 * r['_rel_viol']:.4f}%)")
    return lines


def render_violation_pages(pdf, lines):
    LINES_PER_PAGE = 46
    LINE_H = 0.20    # inches per line
    FV = FontProperties(family="monospace", size=6.5)

    n_pages = max(1, int(np.ceil(len(lines) / LINES_PER_PAGE)))
    for page_i in range(n_pages):
        chunk = lines[page_i * LINES_PER_PAGE:(page_i + 1) * LINES_PER_PAGE]

        fig, ax = plt.subplots(figsize=(17, max(LINE_H * len(chunk) + 0.7, 2.5)))
        ax.axis("off")
        ax.set_xlim(0, 1)
        ax.set_ylim(0, len(chunk))     # one unit per line

        page_lbl = f" (page {page_i + 1}/{n_pages})" if n_pages > 1 else ""
        ax.set_title(f"Bound violations{page_lbl} — {len(lines)} configurations, "
                     "sorted by relative violation within each problem",
                     loc="left", fontproperties=FH, pad=8)

        for i, line in enumerate(chunk):
            ax.text(0, len(chunk) - i - 0.5, line, ha="left", va="center",
                    fontproperties=FV, color="#8b0000")

        plt.tight_layout(pad=0.4)
        pdf.savefig(fig, bbox_inches="tight")
        plt.close(fig)


# ── Generate the PDF ───────────────────────────────────────────────────────────
print(f"\nWriting PDF -> {OUT_PATH}")

MATRICES = [
    (DDTYPES, "DD Type",   "By DD type  -  bounds that hold / bounds checked"),
    (WIDTHS,  "Max Width", "By max width  -  bounds that hold / bounds checked"),
]

n_rows_fig = len(PROBLEMS) + 1
max_cols   = max(len(m[0]) for m in MATRICES)
fig_h = len(MATRICES) * (HDR_H + n_rows_fig * ROW_H + 0.75) + 1.3
fig_w = COL_W_LBL + COL_W * (max_cols + 1) + 0.6

fig, axes = plt.subplots(len(MATRICES), 1, figsize=(fig_w, fig_h))
if len(MATRICES) == 1:
    axes = [axes]

summary = {}
for ax, (cols, field, title) in zip(axes, MATRICES):
    grand = render_matrix(ax, build_matrix(cols, field), cols, title)
    summary[title.split("  -  ")[0]] = grand

pct = lambda s, t: (100 * s / t) if t else 0
grand_all = summary["By DD type"]
fig.suptitle(
    "Validity of the restricted (primal) and relaxed (dual) bounds — DD C++\n"
    f"DD: {src_cpp}   |   exact: {src_exact or '—'}   |   Gurobi: {src_gur}\n"
    "Reference optimum: exact DD (not reduced) when available, otherwise Gurobi with gap 0"
    f"   [SOC Knapsack: exact DD only]\n"
    "Rule (max): Restricted <= optimum <= Relaxed.  (min): Relaxed <= optimum <= Restricted.  "
    f"Relative tolerance {REL_TOL:g}.\n"
    f"{len(checks)} checkable configurations over {len(ref)} instances with a known optimum "
    f"({n_exact} exact DD, {n_gurobi} Gurobi gap 0). "
    f"Each cell: bounds that hold / bounds checked. Violations: {len(violations)}.\n"
    "Python and C++ agree on the diagrams (python_cpp_agreement_table.py), so only C++ is checked.",
    fontproperties=FontProperties(family="monospace", size=8), y=0.995)

plt.tight_layout(rect=[0, 0, 1, 0.93])
with pdf_backend.PdfPages(OUT_PATH) as pdf:
    pdf.savefig(fig, bbox_inches="tight")
    plt.close(fig)

    lines = violation_lines()
    if lines:
        render_violation_pages(pdf, lines)

print("\nSummary:")
for label, (s, t) in summary.items():
    print(f"  {label:<16} {s}/{t}  ({pct(s, t):.2f}%)")

print(f"\nBound violations: {len(violations)}")
if lines:
    for line in lines:
        print(f"  {line}")
else:
    print("  none — every restricted and relaxed bound holds")

print("\nDone!")
