import glob
import os
import re
import shutil
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime

# Larger fonts for legend and axis numbers/labels so the figures stay readable
# when shrunk into the paper (2-up layout).
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

def _file_datetime(path):
    """Extract the datetime from a name '..._DD_MM_YYYY_HH-MM-SS.csv'. The names
    cannot be sorted as strings: the day comes first, so the date must be parsed."""
    m = re.search(r"(\d{2}_\d{2}_\d{4}_\d{2}-\d{2}-\d{2})", os.path.basename(path))
    if not m:
        return datetime.fromtimestamp(os.path.getmtime(path))
    return datetime.strptime(m.group(1), "%d_%m_%Y_%H-%M-%S")

def load_latest(folder, pattern, exclude_grouping=False):
    files = glob.glob(os.path.join(folder, pattern))
    if exclude_grouping:
        files = [f for f in files if "grouping" not in os.path.basename(f).lower()]
    if not files:
        raise FileNotFoundError(f"Could not find '{pattern}' in {folder}")
    path = max(files, key=_file_datetime)
    print(f"  {path}")
    return pd.read_csv(path)

def load_relax(folder, lang):
    """Load the CSV of the relax chosen in RELAX_TYPE.

    - "priority" -> results_<lang>_*.csv (excluding the *_grouping_* ones)
    - "grouping" -> results_<lang>_grouping_*.csv
    """
    if RELAX_TYPE == "grouping":
        return load_latest(folder, f'results_{lang}_grouping_*.csv')
    return load_latest(folder, f'results_{lang}_*.csv', exclude_grouping=True)

def sync_missing_rows(df_cpp: pd.DataFrame, df_python: pd.DataFrame):
    join_keys = ['File_name', 'Max Width', 'Is Reduced', 'DD Type', 'Variables number']

    # Detect which rows are in C++ but not in Python
    cpp_missing_in_python = df_cpp.merge(
        df_python[join_keys], on=join_keys, how='left', indicator=True
    ).query('_merge == "left_only"').drop(columns=['_merge'])

    print(f"📥 Adding {len(cpp_missing_in_python)} rows from C++ to Python")

    # Fill in the missing data for Python
    cpp_missing_in_python = cpp_missing_in_python.copy()
    cpp_missing_in_python['Construction Time'] = missing_time
    cpp_missing_in_python['Timeout'] = True

    # Append to the Python DataFrame
    df_python = pd.concat([df_python, cpp_missing_in_python], ignore_index=True)

    # Detect which rows are in Python but not in C++
    python_missing_in_cpp = df_python.merge(
        df_cpp[join_keys], on=join_keys, how='left', indicator=True
    ).query('_merge == "left_only"').drop(columns=['_merge'])

    print(f"📥 Adding {len(python_missing_in_cpp)} rows from Python to C++")

    # Fill in the missing data for C++
    python_missing_in_cpp = python_missing_in_cpp.copy()
    python_missing_in_cpp['Construction Time'] = missing_time
    python_missing_in_cpp['Timeout'] = True

    # Append to the C++ DataFrame
    df_cpp = pd.concat([df_cpp, python_missing_in_cpp], ignore_index=True)

    return df_cpp, df_python

def scatter_plot(merged_df, title, save=False, output_path=None, group_by='Problem type'):
    real_counts = merged_df[group_by].value_counts().to_dict()
    plt.figure(figsize=(8, 8))

    min_val = 1e-6
    cpp = np.clip(merged_df['Construction Time_cpp'], min_val, None)
    py = np.clip(merged_df['Construction Time_python'], min_val, None)

    # Construction Time values are grouped into logarithmic bins
    bin_resolution = 20  # use 10 for 0.1, 20 for 0.05, etc.
    merged_df['CT_cpp_rounded'] = np.floor(np.log10(cpp) * bin_resolution) / bin_resolution
    merged_df['CT_python_rounded'] = np.floor(np.log10(py) * bin_resolution) / bin_resolution

    grouped = (
        merged_df
        .groupby(['CT_cpp_rounded', 'CT_python_rounded',  group_by])
        .size()
        .reset_index(name='count')
    )

    pretty_markers = ['o', 's', 'D', '^', 'v', 'P', '*', 'X', '<', '>']

    problem_types = sorted(grouped[group_by].unique())

    # Build the marker and color map (a unique color per type)
    colors = plt.colormaps['tab10']
    marker_map = {ptype: pretty_markers[i % len(pretty_markers)] for i, ptype in enumerate(problem_types)}
    color_map = {ptype: colors(i % 10) for i, ptype in enumerate(problem_types)}


    # Graficar cada grupo
    for ptype in problem_types:
        group = grouped[grouped[group_by] == ptype]
        plt.scatter(
            10**group['CT_cpp_rounded'],
            10**group['CT_python_rounded'],
            s=20 + group['count'] * 5,
            label = f"{ptype} ({real_counts.get(ptype, 0)})",
            marker=marker_map[ptype],
            color=color_map[ptype],
            alpha=0.8,
            edgecolors='black',
            linewidths=0.5
        )

    # Logarithmic scales
    plt.xscale('log')
    plt.yscale('log')

    # Axis limits (adjusted to the log scale)
    plt.xlim(1e-3, missing_time)
    plt.ylim(1e-3, missing_time)

    # Reference line (ideal diagonal)
    plt.plot([1e-3, missing_time-10], [1e-3, missing_time-10], linestyle='--', color='gray', label='c++ = python')

    # Labels (the image carries no title)
    plt.xlabel('C++ time (s, log)')
    plt.ylabel('Python time (s, log)')

    # Legend
    legend = plt.legend(loc='upper left')          
    for handle in legend.legend_handles:           
        if hasattr(handle, "set_sizes"):
            handle.set_sizes([60])  

    plt.grid(True)

    plt.tight_layout()

    if save:
        plt.savefig(output_path)
    else:
        plt.show()

### Global parameters used below

# Relax to plot: "priority" or "grouping". For now priority is used.
RELAX_TYPE = "priority"

missing_time = 310
min_variables_default = 100
# SOC Knapsack instances are inherently small (max ~36 vars); override the default threshold
min_variables_by_problem = {
    'SOC Knapsack': 0,
    'Sequencing': 0
}
# Problem types to exclude from all plots (use normalized names, e.g. 'SOC Knapsack', 'Independent Set')
EXCLUDED_PROBLEM_TYPES = ['SOC Knapsack']

csv_folder = './data'
cpp_folder    = os.path.join(csv_folder, 'cpp')
python_folder = os.path.join(csv_folder, 'python')
plots_folder  = './plots/dd_construction_time_comparison'
archive_existing(plots_folder)
file_timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")

try:
    # ---------------------------------------------------------------------------
    # 1) Load the CSVs and sync the missing rows
    # ---------------------------------------------------------------------------
    print(f"🔧 Selected relax: {RELAX_TYPE}")
    df_cpp_without_timeout    = load_relax(cpp_folder,    'cpp')
    df_python_without_timeout = load_relax(python_folder, 'python')

    # Legacy alias: without it the old "Scheduler" rows miss the 'Sequencing' key of
    # min_variables_by_problem and get filtered out by the 100-variable threshold.
    for _df in (df_cpp_without_timeout, df_python_without_timeout):
        _df['Problem type'] = _df['Problem type'].replace({'Scheduler': 'Sequencing'})

    dd_types = df_cpp_without_timeout['DD Type'].unique()   # the DD types present in the CSV

    # Add the rows missing between C++ and Python
    df_cpp, df_python = sync_missing_rows(df_cpp_without_timeout, df_python_without_timeout)

    if EXCLUDED_PROBLEM_TYPES:
        for df in [df_cpp, df_python]:
            df.drop(df[df['Problem type'].isin(EXCLUDED_PROBLEM_TYPES)].index, inplace=True)

    # ---------------------------------------------------------------------------
    # 2) Initial filtering by instance size (threshold per problem type)
    # ---------------------------------------------------------------------------
    def filter_min_vars(df):
        thresholds = df['Problem type'].map(
            lambda p: min_variables_by_problem.get(p, min_variables_default)
        )
        return df[df['Variables number'] >= thresholds]

    df_cpp    = filter_min_vars(df_cpp)
    df_python = filter_min_vars(df_python)


    if df_cpp.empty or df_python.empty:
        raise ValueError("❌ No data left after filtering by Variables number >= 100")


    # ---------------------------------------------------------------------------
    # 3) Single merge (already filtered)
    # ---------------------------------------------------------------------------
    merged_df = pd.merge(
        df_cpp[['File_name', 'Construction Time', 'Max Width', 'Is Reduced',
                'DD Type', 'Variables number', 'Problem type']],
        df_python[['File_name', 'Construction Time', 'Max Width', 'Is Reduced',
                'DD Type', 'Variables number', 'Problem type']],
        on=['File_name', 'Max Width', 'Is Reduced', 'DD Type'],
        suffixes=('_cpp', '_python')
    )

    if merged_df.empty:
        raise ValueError("❌ The merge came out empty; check the filters and the join keys.")

    # Make sure the unified problem-type column is present
    merged_df['Problem type'] = merged_df['Problem type_cpp']
    merged_df = merged_df.drop(columns=['Problem type_cpp', 'Problem type_python'])

    # ---------------------------------------------------------------------------
    # 4) Plot each DD Type (3 main plots)
    # ---------------------------------------------------------------------------
    # Friendly name for the plot titles (does not affect the file names)
    DD_LABEL = {"RelaxPriority": "Relax", "RelaxGrouping": "Relax (grouping)"}

    for dd_type in dd_types:
        sub_df = merged_df[merged_df['DD Type'] == dd_type].copy()

        if sub_df.empty:
            print(f"⏭️  No data for DD Type: {dd_type} with Variables number >= {min_variables_default}")
            continue

        output_path = f'{plots_folder}/construction_time_{dd_type.lower().replace(" ", "_")}_{file_timestamp}.png'
        print(f"📊 Generating plot for DD Type: {dd_type}")

        scatter_plot(
            sub_df,
            title=f'Construction Time comparison - DD Type: {DD_LABEL.get(dd_type, dd_type)}',
            save=True,
            output_path=output_path
        )

    # ---------------------------------------------------------------------------
    # 5) Plot Relaxed and Restricted by width (6 secondary plots)
    # ---------------------------------------------------------------------------
    problem_types = df_cpp['Problem type'].unique()

    for dd_type in dd_types:
        if dd_type == "Exact":
            continue

        for problem_type in problem_types:
            sub_df = merged_df[
                (merged_df['DD Type']     == dd_type) &
                (merged_df['Problem type'] == problem_type)
            ].copy()  

            if sub_df.empty:
                print(f"⏭️  No data for DD Type: {dd_type} and Problem type: {problem_type}")
                continue

            out_name = f"construction_time_{dd_type.lower().replace(' ', '_')}_{problem_type.lower().replace(' ', '_')}_{file_timestamp}.png"
            output_path = f"{plots_folder}/{out_name}"

            print(f"📊 Generating plot for DD Type: {dd_type} and Problem type: {problem_type}")

            scatter_plot(
                sub_df,
                title=f"Construction Time comparison\nDD Type: {DD_LABEL.get(dd_type, dd_type)} • Problem type: {problem_type}",
                save=True,
                output_path=output_path,
                group_by='Max Width'          # ← the legend will show the different widths
            )

except Exception as e:
    print(f"Error reading: {e}")

