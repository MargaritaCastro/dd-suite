"""Compares the GROUPING relaxation against the PRIORITY one (both live in
results_<lang>_*.csv, told apart by 'DD Type').

Per language, 2 scatter plots: construction time, and bound gap % vs the Exact DD
optimum. Each point is an (instance, Max Width) pair; below the dashed diagonal
means grouping is better.
"""

import glob
import os
import re
import shutil
from datetime import datetime

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# Larger fonts for legend and axis numbers/labels so the figures stay readable
# when shrunk into the paper.
plt.rcParams.update({
    "legend.fontsize": 15,
    "xtick.labelsize": 16,
    "ytick.labelsize": 16,
    "axes.labelsize": 18,
})


def archive_existing(folder):
    """Move the existing outputs of 'folder' into its 'deprecated/' subfolder."""
    deprecated = os.path.join(folder, "deprecated")
    os.makedirs(deprecated, exist_ok=True)
    for path in glob.glob(os.path.join(folder, "*")):
        if os.path.isfile(path):
            shutil.move(path, os.path.join(deprecated, os.path.basename(path)))


BASE          = os.path.dirname(os.path.abspath(__file__))
PY_DIR        = os.path.join(BASE, "data", "python")
CPP_DIR       = os.path.join(BASE, "data", "cpp")
PLOTS_DIR     = os.path.join(BASE, "plots", "grouping_vs_priority")
archive_existing(PLOTS_DIR)
ts            = datetime.now().strftime("%Y%m%d_%H%M%S")

WIDTHS        = [500, 1000, 2000, 5000, 10000, 20000]
PRIORITY_TYPE = "RelaxPriority"  # priority-based relax (discard/merge by priority)
GROUPING_TYPE = "RelaxGrouping"  # grouping-based relax
EXACT_TYPE    = "Exact"          # exact DD -> reference optimum for the gap


# ── Loading helpers ─────────────────────────────────────────────────────────────

def _file_datetime(path):
    """Extract the datetime from a name '..._DD_MM_YYYY_HH-MM-SS.csv'. The names
    cannot be sorted as strings: the day comes first, so the date must be parsed."""
    m = re.search(r"(\d{2}_\d{2}_\d{4}_\d{2}-\d{2}-\d{2})", os.path.basename(path))
    if not m:
        return datetime.fromtimestamp(os.path.getmtime(path))
    return datetime.strptime(m.group(1), "%d_%m_%Y_%H-%M-%S")

def load_latest(folder, pattern):
    files = glob.glob(os.path.join(folder, pattern))
    if not files:
        return None
    path = max(files, key=_file_datetime)
    df = pd.read_csv(path)
    df.columns = df.columns.str.strip()
    for col in df.columns:
        if df[col].dtype == object:
            df[col] = df[col].astype(str).str.replace("\x00", "", regex=False).str.strip()
    for col in ["Construction Time", "Max Width", "Optimization Value", "Variables number"]:
        if col in df.columns:
            df[col] = pd.to_numeric(df[col], errors="coerce")
    print(f"  {os.path.basename(path)}  ({len(df)} rows)")
    return df


_PROBLEM_TYPE_MAP = {"IndependentSet": "Independent Set", "Set Covering": "Set Cover",
                     # Legacy alias: CSVs predating the rename carry "Scheduler".
                     "Scheduler": "Sequencing"}

def norm_problem_type(s):
    return _PROBLEM_TYPE_MAP.get(str(s).strip(), str(s).strip())

def norm_key(s):
    """Strip known extensions and unify set_covering_ -> set_cover_."""
    s = str(s).strip()
    for ext in (".txt", ".clq", ".col"):
        if s.endswith(ext):
            s = s[: -len(ext)]
            break
    return s.replace("set_covering_", "set_cover_")


# ── Reference optima (Gurobi) ───────────────────────────────────────────────────
# Gurobi and the Exact DD give the same optimum; they are combined to maximize coverage.

print("Loading Gurobi reference:")
df_gpy = load_latest(PY_DIR, "results_gurobi_python_*.csv")
if df_gpy is None:
    GUROBI_REF = {}
    print("  (no Gurobi file; only the Exact DD will be used)")
else:
    df_gpy["File_key"]     = df_gpy["File_name"].apply(norm_key)
    df_gpy["Problem type"] = df_gpy["Problem type"].apply(norm_problem_type)
    df_gpy["Gurobi_Sol"]   = pd.to_numeric(df_gpy["Best solution"], errors="coerce")
    GUROBI_REF = (df_gpy.dropna(subset=["Gurobi_Sol"])
                  .groupby(["File_key", "Problem type"])["Gurobi_Sol"].mean().to_dict())
    print(f"  Gurobi reference: {len(GUROBI_REF)} instances")


# ── Building the per-language comparison DataFrame ──────────────────────────────

def build_comparison(folder, prefix):
    """Return a DataFrame with one row per (instance, Max Width) holding the
    *_priority and *_grouping columns of Construction Time and Gap %."""
    df = load_latest(folder, f"results_{prefix}_*.csv")

    if df is None:
        print(f"  ⏭️  No results file for {prefix}")
        return None

    def prep(dd_type):
        sub = df[(df["DD Type"] == dd_type) & (df["Max Width"].isin(WIDTHS))].copy()
        sub["File_key"]     = sub["File_name"].apply(norm_key)
        sub["Problem type"] = sub["Problem type"].apply(norm_problem_type)
        # Average repetitions of the same (instance, width) if there are any
        return (sub.groupby(["File_key", "Problem type", "Max Width"], as_index=False)
                   .agg({"Construction Time": "mean", "Optimization Value": "mean"}))

    prio = prep(PRIORITY_TYPE)
    grp  = prep(GROUPING_TYPE)

    if prio.empty or grp.empty:
        print(f"  ⏭️  No {PRIORITY_TYPE}/{GROUPING_TYPE} rows for {prefix}")
        return None

    # Reference optima: the Exact DD value, with Gurobi filling in the rest.
    # No reference => NaN gap and the row is discarded.
    exact = df[df["DD Type"] == EXACT_TYPE].copy()
    exact["File_key"]     = exact["File_name"].apply(norm_key)
    exact["Problem type"] = exact["Problem type"].apply(norm_problem_type)
    exact_ref = (exact.groupby(["File_key", "Problem type"])["Optimization Value"]
                 .mean().to_dict())
    ref = {**GUROBI_REF, **exact_ref}   # Exact overrides Gurobi where both exist
    print(f"  Reference optima: {len(ref)} instances "
          f"(Exact {len(exact_ref)} + Gurobi {len(GUROBI_REF)})")

    merged = prio.merge(grp, on=["File_key", "Problem type", "Max Width"],
                        suffixes=("_priority", "_grouping"))
    if merged.empty:
        print(f"  ⏭️  The priority/grouping merge came out empty for {prefix}")
        return None

    # Absolute gap with respect to the optimum: |bound - opt| / |opt|. Always >= 0
    # so that the plot lives in the first quadrant.
    def gap_pct(row, col):
        opt = ref.get((row["File_key"], row["Problem type"]), np.nan)
        val = row[col]
        if pd.isna(opt) or pd.isna(val) or opt == 0:
            return np.nan
        return abs(val - opt) / abs(opt) * 100.0

    merged["Gap_priority"] = merged.apply(lambda r: gap_pct(r, "Optimization Value_priority"), axis=1)
    merged["Gap_grouping"] = merged.apply(lambda r: gap_pct(r, "Optimization Value_grouping"), axis=1)

    print(f"  ✅ {prefix}: {len(merged)} (instance, width) pairs compared")
    return merged


# ── Generic scatter colored by a category ───────────────────────────────────────

WIDTH_CMAP  = plt.colormaps["viridis"]
WIDTH_COLOR = {w: WIDTH_CMAP(i / max(1, len(WIDTHS) - 1)) for i, w in enumerate(WIDTHS)}
PROB_CMAP   = plt.colormaps["tab10"]

def scatter_compare(df, x_col, y_col, color_col, title, xlabel, ylabel,
                    output_path, log_scale=False, floor=None, label_prefix=""):
    plt.figure(figsize=(8, 8))

    data = df.dropna(subset=[x_col, y_col]).copy()
    if data.empty:
        print(f"  ⏭️  No data for '{title}'")
        plt.close()
        return

    x = data[x_col].to_numpy(dtype=float)
    y = data[y_col].to_numpy(dtype=float)
    if floor is not None:                       # avoid zeros/negatives on a log scale
        x = np.clip(x, floor, None)
        y = np.clip(y, floor, None)

    # Fixed ordering and palette according to the color column
    if color_col == "Max Width":
        categories = [w for w in WIDTHS if (data[color_col] == w).any()]
        color_of = lambda c: WIDTH_COLOR[c]
    else:
        categories = sorted(data[color_col].unique())
        color_of = lambda c: PROB_CMAP(categories.index(c) % 10)

    for c in categories:
        m = (data[color_col] == c).to_numpy()
        if not m.any():
            continue
        plt.scatter(x[m], y[m],
                    s=28, color=color_of(c), alpha=0.7,
                    edgecolors="black", linewidths=0.3,
                    label=f"{label_prefix}{c} ({int(m.sum())})")

    # Reference diagonal priority == grouping
    lo = float(min(x.min(), y.min()))
    hi = float(max(x.max(), y.max()))
    plt.plot([lo, hi], [lo, hi], ls="--", color="gray", lw=1, label="grouping = priority")

    if log_scale:
        plt.xscale("log")
        plt.yscale("log")

    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.grid(True, alpha=0.3)
    leg = plt.legend(loc="best")
    for h in leg.legend_handles:
        if hasattr(h, "set_sizes"):
            h.set_sizes([50])
    plt.tight_layout()
    plt.savefig(output_path, dpi=150)
    plt.close()
    print(f"  💾 Saved: {output_path}")


# ── Mean gap vs width (one line per relax type) ─────────────────────────────────

RELAX_COLORS = {"priority": "#1f77b4", "grouping": "#d62728"}
IQR_WHISKER  = 1.5   # Tukey rule: discard outside [Q1-1.5*IQR, Q3+1.5*IQR]

def _iqr_filter(vals):
    """Drop outliers with the IQR rule; if nothing is left, keep the original."""
    vals = vals.dropna()
    if len(vals) < 4:                       # too little data to define outliers
        return vals, 0
    q1, q3 = vals.quantile(0.25), vals.quantile(0.75)
    iqr = q3 - q1
    lo, hi = q1 - IQR_WHISKER * iqr, q3 + IQR_WHISKER * iqr
    kept = vals[(vals >= lo) & (vals <= hi)]
    if kept.empty:
        return vals, 0
    return kept, len(vals) - len(kept)

def gap_vs_width(merged, lang_label, prefix):
    """One file per example: gap% vs Max Width, one line per relax type. Outliers are
    dropped with the IQR rule; the band is the p25-p75 of what remains."""
    for problem in sorted(merged["Problem type"].unique()):
        sub = merged[merged["Problem type"] == problem]
        plt.figure(figsize=(8, 6))

        plotted = False
        total_dropped = 0
        for kind, col in [("priority", "Gap_priority"), ("grouping", "Gap_grouping")]:
            widths, means, q25s, q75s = [], [], [], []
            for w in WIDTHS:
                kept, dropped = _iqr_filter(sub.loc[sub["Max Width"] == w, col])
                if kept.empty:
                    continue
                total_dropped += dropped
                widths.append(w)
                means.append(kept.mean())
                q25s.append(kept.quantile(0.25))
                q75s.append(kept.quantile(0.75))
            if not widths:
                continue
            color = RELAX_COLORS[kind]
            plt.fill_between(widths, q25s, q75s, color=color, alpha=0.15)
            plt.plot(widths, means, marker="o", color=color, label=f"relax {kind}")
            plotted = True

        if not plotted:
            plt.close()
            continue

        plt.axhline(0, color="black", ls=":", lw=1)
        plt.xscale("log")
        plt.xticks(WIDTHS, [str(w) for w in WIDTHS])
        plt.minorticks_off()
        plt.xlabel("Max width")
        plt.ylabel("Gap (%)")
        plt.grid(True, alpha=0.3)
        plt.legend(loc="best")
        plt.tight_layout()

        safe_problem = problem.lower().replace(" ", "_")
        output_path = os.path.join(
            PLOTS_DIR, f"grouping_vs_priority_gap_{prefix}_{safe_problem}_{ts}.png")
        plt.savefig(output_path, dpi=150)
        plt.close()
        print(f"  💾 Saved: {output_path}")


# ── Generation ──────────────────────────────────────────────────────────────────

LANGS = [("python", "Python", PY_DIR), ("cpp", "C++", CPP_DIR)]

for prefix, lang_label, folder in LANGS:
    print(f"\n=== {lang_label} ===")
    merged = build_comparison(folder, prefix)
    if merged is None:
        continue

    # 1) Construction time (log-log; grouping can be very small)
    #    colored by Problem type
    scatter_compare(
        merged,
        x_col="Construction Time_priority",
        y_col="Construction Time_grouping",
        color_col="Problem type",
        title=f"{lang_label} — Construction time: grouping vs priority\n"
              "(below the diagonal => grouping is faster)",
        xlabel="Construction Time priority (s)",
        ylabel="Construction Time grouping (s)",
        output_path=os.path.join(PLOTS_DIR, f"grouping_vs_priority_time_{prefix}_{ts}.png"),
        log_scale=True,
        floor=1e-6,
    )

    # 2) Bound gap (%) vs Max Width, one line per relax type.
    #    One file per example (Problem type).
    gap_vs_width(merged, lang_label, prefix)

print("\nDone!")
