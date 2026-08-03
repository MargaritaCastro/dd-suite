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
    """Create 'folder' and its 'deprecated/' and move any existing outputs there
    before generating new ones, so old versions do not pile up in the root."""
    deprecated = os.path.join(folder, "deprecated")
    os.makedirs(deprecated, exist_ok=True)
    for path in glob.glob(os.path.join(folder, "*")):
        if os.path.isfile(path):
            shutil.move(path, os.path.join(deprecated, os.path.basename(path)))

### Global parameters
# This script reproduces the paper table "Average ratio of construction times per
# DD type": the arithmetic mean of the construction-time ratio over every
# (instance, width) pair, computed on the SAME pipeline as the scatter plots of
# dd_construction_time_comparison.py (Python vs C++) and rust_plots.py (C++ vs ddo).
# The full pipeline is documented at the bottom of this file.
csv_folder    = './data'
cpp_folder    = os.path.join(csv_folder, 'cpp')
python_folder = os.path.join(csv_folder, 'python')
rust_folder   = os.path.join(csv_folder, 'rust')
output_folder = './tables/construction_time_ratio_table'
archive_existing(output_folder)
file_timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")

# Relax used for C++/Python: "priority" or "grouping" (ddo implements only one).
RELAX_TYPE = "priority"
RELAX_DD_TYPE = {"priority": "RelaxPriority", "grouping": "RelaxGrouping"}

# Per-construction time limit used in the experiments (5 min). Pairs where either
# implementation hits it are EXCLUDED from the average: the censored time (drawn
# as missing_time = 310 s in the scatter plots) is not the real time and would
# bias the ratio. A pair counts as censored when it is missing from one of the
# CSVs (the run did not finish) or when its time exceeds the threshold.
TIME_LIMIT_SECONDS = 300

min_variables_default = 100
# SOC Knapsack instances are inherently small (max ~36 vars); override the default threshold
min_variables_by_problem = {
    'SOC Knapsack': 0,
    'Sequencing': 0,
}
# Problem types to exclude from the table (use normalized names, e.g. 'SOC Knapsack')
EXCLUDED_PROBLEM_TYPES = ['SOC Knapsack']

# Instances excluded from the C++ / ddo block ONLY: for them ddo returns a
# degenerate width-1 DD in ~0.015 s for every width (an invalid data point that on
# its own pushed the restricted/relaxed average from ~2.5 to ~7).
EXCLUDED_INSTANCES_RUST = ['set_cover_n4000_m3836_d0.02_b_w165_seed1']

# Row order of the table
DD_TYPE_ORDER = ['Exact', 'Restricted', 'Relaxed']
COMPARISONS = ['Python / C++', 'C++ / ddo']
# Figure label: 'ddo' in typewriter font, like the \texttt{ddo} of the paper.
COMPARISON_LABEL_FIG = {'C++ / ddo': r'C++ / $\mathtt{ddo}$'}


def _file_datetime(path):
    """Extract the datetime from a '..._DD_MM_YYYY_HH-MM-SS.csv' name.

    The names cannot be sorted as strings: the day comes first, so '27_05_2026'
    (27 May) would sort after '01_06_2026' (1 Jun). The real date must be parsed.
    """
    m = re.search(r"(\d{2}_\d{2}_\d{4}_\d{2}-\d{2}-\d{2})", os.path.basename(path))
    if not m:
        return datetime.fromtimestamp(os.path.getmtime(path))
    return datetime.strptime(m.group(1), "%d_%m_%Y_%H-%M-%S")

def load_latest(folder, pattern, exclude_grouping=False):
    """Return (df, file name); the name is reported in the table header so it is
    always traceable which run the averages come from."""
    files = glob.glob(os.path.join(folder, pattern))
    if exclude_grouping:
        files = [f for f in files if "grouping" not in os.path.basename(f).lower()]
    if not files:
        raise FileNotFoundError(f"Could not find '{pattern}' in {folder}")
    path = max(files, key=_file_datetime)
    df = pd.read_csv(path)
    print(f"  {path}  ({len(df)} rows)")
    return df, os.path.basename(path)

def load_relax(folder, lang):
    """Load the CSV of the relax selected in RELAX_TYPE.

    - "priority" -> results_<lang>_*.csv (excluding the *_grouping_* ones)
    - "grouping" -> results_<lang>_grouping_*.csv
    """
    if RELAX_TYPE == "grouping":
        return load_latest(folder, f'results_{lang}_grouping_*.csv')
    return load_latest(folder, f'results_{lang}_*.csv', exclude_grouping=True)

def normalize_problem_types(df):
    """Unify the problem names across the CSVs of the three languages."""
    name_map = {
        "IndependentSet": "Independent Set",
        "Set Covering": "Set Cover",
        "SOCKnapsack": "SOC Knapsack",
        # Legacy alias: CSVs from before the problem was renamed carry "Scheduler".
        "Scheduler": "Sequencing",
    }
    df['Problem type'] = df['Problem type'].replace(name_map)
    return df

def filter_min_vars(df):
    thresholds = df['Problem type'].map(
        lambda p: min_variables_by_problem.get(p, min_variables_default)
    )
    return df[df['Variables number'] >= thresholds]

def standardize_relax(df):
    """Keep a single relax (the one selected in RELAX_TYPE) and rename it to 'Relaxed'.

    The C++/Python CSVs carry both relaxations ('RelaxPriority' and 'RelaxGrouping')
    in the same file; ddo implements only one, which it calls 'Relaxed'. Without
    standardising, the merge against ddo would drop every relax row.
    """
    keep = RELAX_DD_TYPE[RELAX_TYPE]
    other = {'RelaxPriority', 'RelaxGrouping'} - {keep}
    df = df[~df['DD Type'].isin(other)].copy()
    df['DD Type'] = df['DD Type'].replace({keep: 'Relaxed'})
    return df

def prepare(df, language):
    """Filters shared by the three languages, in the same order as the scatter plots."""
    df = normalize_problem_types(df.copy())
    df = df[~df['Problem type'].isin(EXCLUDED_PROBLEM_TYPES)]
    df = filter_min_vars(df)
    df = standardize_relax(df) if language != 'Rust' else df

    # Keys comparable across languages: ddo writes the file name without extension
    # and uses 'set_covering_' instead of 'set_cover_'; besides, C++/Python store
    # the "infinite" width of the exact DD as 2147483646 and ddo as 99999.
    df['File_name'] = df['File_name'].astype(str).apply(lambda x: os.path.splitext(x)[0] + '.txt')
    df['File_name'] = df['File_name'].str.replace(r'^set_covering_', 'set_cover_', regex=True)
    df.loc[df['DD Type'] == 'Exact', 'Max Width'] = 99999
    df['Is Reduced'] = df['Is Reduced'].astype(str).str.lower().map(
        {'true': True, 'false': False}
    )
    return df[['File_name', 'Problem type', 'Variables number', 'DD Type',
               'Max Width', 'Is Reduced', 'Construction Time']]

def compute_ratios(df_slow, df_fast, slow_suffix, fast_suffix, match_reduced,
                   excluded_instances=()):
    """Join both implementations and return (ratios, stats), where the ratio is
    slow_time / fast_time per (instance, width, DD type) pair.

    - The merge is inner on purpose: pairs that exist in only one CSV are runs that
      did not finish within the 5-minute limit, and their censored time would bias
      the average. Those pairs are the "only" counts reported in `stats`.
    - `match_reduced` is False when comparing against ddo, which does not implement
      reduction (its CSV only carries rows with Is Reduced = false).
    """
    keys = ['File_name', 'Max Width', 'DD Type']
    if match_reduced:
        keys.append('Is Reduced')

    if excluded_instances:
        def drop_excluded(df):
            stems = df['File_name'].apply(lambda x: os.path.splitext(x)[0])
            return df[~stems.isin(excluded_instances)]
        df_slow, df_fast = drop_excluded(df_slow), drop_excluded(df_fast)

    # Configurations present on one side only. They are counted over the set of
    # keys (not over the rows) so that keys repeated within a CSV do not inflate
    # the count.
    keys_slow = set(map(tuple, df_slow[keys].itertuples(index=False, name=None)))
    keys_fast = set(map(tuple, df_fast[keys].itertuples(index=False, name=None)))
    only_slow = len(keys_slow - keys_fast)
    only_fast = len(keys_fast - keys_slow)

    merged = pd.merge(df_slow, df_fast, on=keys,
                      suffixes=(f'_{slow_suffix}', f'_{fast_suffix}'))
    merged['Problem type'] = merged[f'Problem type_{slow_suffix}']

    slow_time = merged[f'Construction Time_{slow_suffix}']
    fast_time = merged[f'Construction Time_{fast_suffix}']

    # Pairs censored by the time limit are dropped, and so are non-positive times
    # (measurements below the clock resolution, which would give absurd ratios).
    valid = (slow_time < TIME_LIMIT_SECONDS) & (fast_time < TIME_LIMIT_SECONDS) \
            & (slow_time > 0) & (fast_time > 0)
    dropped = int((~valid).sum())
    if dropped:
        print(f"  Dropping {dropped} pairs (timeout or time <= 0)")

    merged = merged[valid].copy()
    merged['Ratio'] = merged[f'Construction Time_{slow_suffix}'] / merged[f'Construction Time_{fast_suffix}']

    stats = {'pairs': len(merged), 'only_slow': only_slow, 'only_fast': only_fast,
             'dropped': dropped}
    return merged, stats

def summarize(merged, comparison_label):
    """Average ratio per DD type, with the number of pairs backing it."""
    rows = []
    for dd_type in DD_TYPE_ORDER:
        sub = merged[merged['DD Type'] == dd_type]
        rows.append({
            'Comparison': comparison_label,
            'DD type': dd_type,
            'Ratio': sub['Ratio'].mean() if not sub.empty else float('nan'),
            'n': len(sub),
        })
    return pd.DataFrame(rows)


try:
    # ---------------------------------------------------------------------------
    # 1) Load CSVs
    # ---------------------------------------------------------------------------
    print(f"Selected relax (C++/Python): {RELAX_TYPE}")
    print("Loading C++:")
    df_cpp,    src_cpp    = load_relax(cpp_folder,    'cpp')
    print("Loading Python:")
    df_python, src_python = load_relax(python_folder, 'python')
    print("Loading ddo (Rust):")
    df_rust,   src_rust   = load_latest(rust_folder,  'results_rust_*.csv')

    # ---------------------------------------------------------------------------
    # 2) Shared filters (same pipeline as the scatter plots)
    # ---------------------------------------------------------------------------
    df_cpp    = prepare(df_cpp,    'C++')
    df_python = prepare(df_python, 'Python')
    df_rust   = prepare(df_rust,   'Rust')

    if df_cpp.empty or df_python.empty or df_rust.empty:
        raise ValueError("No data left after the filters; check the CSVs and the thresholds.")

    # ---------------------------------------------------------------------------
    # 3) Compute the ratios of each block
    # ---------------------------------------------------------------------------
    print("\nPython / C++:")
    ratios_py_cpp, stats_py_cpp = compute_ratios(df_python, df_cpp, 'python', 'cpp',
                                                 match_reduced=True)

    # Against ddo only the non-reduced C++ rows are used: ddo does not reduce, and
    # without this filter every ddo row would match twice (reduced and non-reduced).
    print("C++ / ddo:")
    ratios_cpp_rust, stats_cpp_rust = compute_ratios(
        df_cpp[df_cpp['Is Reduced'] == False], df_rust, 'cpp', 'rust',
        match_reduced=False, excluded_instances=EXCLUDED_INSTANCES_RUST)

    df_table = pd.concat([
        summarize(ratios_py_cpp,   COMPARISONS[0]),
        summarize(ratios_cpp_rust, COMPARISONS[1]),
    ], ignore_index=True)

    # ---------------------------------------------------------------------------
    # 4) Print the table on the console
    # ---------------------------------------------------------------------------
    print()
    print(f"Files used: Python={src_python} | C++={src_cpp} | ddo={src_rust}")
    print(f"  Python / C++: {stats_py_cpp['pairs']:,} common pairs "
          f"(Python only: {stats_py_cpp['only_slow']:,}, C++ only: {stats_py_cpp['only_fast']:,})")
    print(f"  C++ / ddo:    {stats_cpp_rust['pairs']:,} common pairs "
          f"(C++ only: {stats_cpp_rust['only_slow']:,}, ddo only: {stats_cpp_rust['only_fast']:,})")
    print()
    print(f"{'Comparison':<14} {'DD type':<12} {'Avg ratio':>10} {'n pairs':>9}")
    print("-" * 48)
    for i, row in df_table.iterrows():
        if i == len(DD_TYPE_ORDER):
            print("-" * 48)
        ratio = "—" if pd.isna(row['Ratio']) else f"{row['Ratio']:.1f}"
        print(f"{row['Comparison']:<14} {row['DD type']:<12} {ratio:>10} {row['n']:>9,}")
    print("-" * 48)
    print()

    # Breakdown per problem, useful to check where each average comes from
    print("Breakdown per problem type (average ratio / n pairs):")
    for label, merged in [(COMPARISONS[0], ratios_py_cpp), (COMPARISONS[1], ratios_cpp_rust)]:
        print(f"  {label}")
        breakdown = merged.groupby(['Problem type', 'DD Type'])['Ratio'].agg(['mean', 'size'])
        for (problem, dd_type), stats in breakdown.iterrows():
            print(f"    {problem:<18} {dd_type:<12} {stats['mean']:>6.1f}  ({int(stats['size']):,})")
    print()

    # ---------------------------------------------------------------------------
    # 5) Render the figure
    # ---------------------------------------------------------------------------
    # Header with the provenance of the data: which CSV each implementation comes
    # from, and how many configurations were left out because they exist on one
    # side only ("only"), i.e. the ones that did not finish within the 5-min limit.
    source_lines = [
        f"Python: {src_python}   |   C++: {src_cpp}   |   ddo: {src_rust}",
        f"Python / C++: {stats_py_cpp['pairs']:,} pairs "
        f"(Python only: {stats_py_cpp['only_slow']:,}, C++ only: {stats_py_cpp['only_fast']:,})"
        f"   |   C++ / ddo: {stats_cpp_rust['pairs']:,} pairs "
        f"(C++ only: {stats_cpp_rust['only_slow']:,}, ddo only: {stats_cpp_rust['only_fast']:,})",
    ]

    n_rows        = len(df_table)
    row_height    = 0.28
    header_h      = 0.36
    source_size   = 6.5
    source_line_h = 0.22
    source_h      = source_line_h * len(source_lines) + 0.10   # text band above the table
    total_h       = header_h + n_rows * row_height

    # Column widths: comparison | DD type | ratio
    col_widths = [1.6, 1.3, 3.1]
    total_w    = sum(col_widths)

    # The provenance lines are usually wider than the table itself; the columns are
    # scaled up so the rules span at least that width and the block stays aligned
    # (otherwise the text overflows the axes and bbox_inches="tight" leaves the
    # table hanging on the left of the image).
    # 0.62 em per character is the advance width of the default monospace face,
    # plus a small right margin so the longest line never touches the edge.
    source_w = max(len(line) for line in source_lines) * 0.62 * source_size / 72 + 0.10
    if source_w > total_w:
        scale = source_w / total_w
        col_widths = [w * scale for w in col_widths]
        total_w = sum(col_widths)

    # The axes occupies the whole figure so that one inch of axes is one inch of
    # figure: the provenance text has a fixed size in points, and with the default
    # padding it would stick out of the rules. bbox_inches="tight" trims the margin.
    fig = plt.figure(figsize=(total_w, total_h + source_h))
    ax  = fig.add_axes([0, 0, 1, 1])
    ax.axis("off")

    col_left  = [sum(col_widths[:i]) / total_w for i in range(len(col_widths))]
    col_right = [col_left[i] + col_widths[i] / total_w for i in range(len(col_widths))]
    col_center = [(col_left[i] + col_right[i]) / 2 for i in range(len(col_widths))]

    # The vertical coordinates are expressed relative to the table (y = 0 at the
    # bottomrule, y = 1 at the toprule); the provenance band lives above y = 1.
    axes_h   = total_h + source_h
    def table_y(y):
        return y * total_h / axes_h

    header_y = table_y((total_h - header_h / 2) / total_h)
    row_y    = [table_y((total_h - header_h - r * row_height - row_height / 2) / total_h)
                for r in range(n_rows)]

    font_normal = FontProperties(family="serif", size=8.5)
    font_header = FontProperties(family="serif", size=8.5, weight="bold")
    font_source = FontProperties(family="monospace", size=source_size)

    def write(x, y, text, ha, font):
        ax.text(x, y, text, ha=ha, va="center",
                fontproperties=font, transform=ax.transAxes)

    # -- Provenance band, above the toprule --
    for i, line in enumerate(source_lines):
        y = table_y(1.0) + (source_h - 0.10 - (i + 0.5) * source_line_h) / axes_h
        write(0.0, y, line, "left", font_source)

    # -- Header --
    write(col_left[0],   header_y, "Comparison", "left",   font_header)
    write(col_left[1],   header_y, "DD type",    "left",   font_header)
    write(col_center[2], header_y, "Average construction-time ratio", "center", font_header)

    # -- Data rows --
    for r, (_, row) in enumerate(df_table.iterrows()):
        yc = row_y[r]
        label = COMPARISON_LABEL_FIG.get(row['Comparison'], row['Comparison'])
        write(col_left[0],   yc, label, "left", font_normal)
        write(col_left[1],   yc, row['DD type'], "left", font_normal)
        ratio = "—" if pd.isna(row['Ratio']) else f"{row['Ratio']:.1f}"
        write(col_center[2], yc, ratio, "center", font_normal)

    # -- Horizontal rules (booktabs style) --
    def hline(y, lw, x0=0, x1=1):
        ax.plot([x0, x1], [y, y], color="black", linewidth=lw,
                transform=ax.transAxes, clip_on=False)

    hline(table_y(1.0), 1.5)                                  # toprule
    hline(table_y((total_h - header_h) / total_h), 1.0)        # midrule below the header
    hline(row_y[len(DD_TYPE_ORDER) - 1] - row_height / 2 / axes_h, 0.5)  # block divider
    hline(0.0, 1.5)                                           # bottomrule

    os.makedirs(output_folder, exist_ok=True)
    output_path = os.path.join(output_folder, f"construction_time_ratio_{file_timestamp}.png")
    plt.savefig(output_path, dpi=180, bbox_inches="tight")
    print(f"Table saved to: {output_path}")

except Exception as e:
    print(f"Error: {e}")

