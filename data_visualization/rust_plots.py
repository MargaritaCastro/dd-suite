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
    """Crea 'folder' y su 'deprecated/' y mueve ahí las salidas ya existentes
    antes de generar nuevas, para no acumular versiones viejas en la raíz."""
    deprecated = os.path.join(folder, "deprecated")
    os.makedirs(deprecated, exist_ok=True)
    for path in glob.glob(os.path.join(folder, "*")):
        if os.path.isfile(path):
            shutil.move(path, os.path.join(deprecated, os.path.basename(path)))

def _file_datetime(path):
    """Extrae el datetime del nombre '..._DD_MM_YYYY_HH-MM-SS.csv'.

    No se puede ordenar los nombres como strings: el dia va primero, asi que
    '27_05_2026' (27 may) ordenaria despues de '01_06_2026' (1 jun). Hay que
    parsear la fecha real.
    """
    m = re.search(r"(\d{2}_\d{2}_\d{4}_\d{2}-\d{2}-\d{2})", os.path.basename(path))
    if not m:
        return datetime.fromtimestamp(os.path.getmtime(path))
    return datetime.strptime(m.group(1), "%d_%m_%Y_%H-%M-%S")

def load_latest(folder, pattern, exclude_grouping=False):
    files = glob.glob(os.path.join(folder, pattern))
    if exclude_grouping:
        files = [f for f in files if "grouping" not in os.path.basename(f).lower()]
    if not files:
        raise FileNotFoundError(f"No se encontro '{pattern}' en {folder}")
    path = max(files, key=_file_datetime)
    print(f"  {path}")
    return pd.read_csv(path)

def load_relax(folder, lang):
    """Carga el CSV del relax elegido en RELAX_TYPE (solo C++/Python; Rust no
    implementa grouping y siempre usa su único CSV).

    - "priority" -> results_<lang>_*.csv (excluyendo los *_grouping_*)
    - "grouping" -> results_<lang>_grouping_*.csv
    """
    if RELAX_TYPE == "grouping":
        return load_latest(folder, f'results_{lang}_grouping_*.csv')
    return load_latest(folder, f'results_{lang}_*.csv', exclude_grouping=True)

def standardize_relax(df):
    """Deja un único relax (el elegido en RELAX_TYPE) y lo renombra a 'Relaxed'
    para que calce con Rust, que solo implementa un relax.

    Los CSV de C++/Python traen dos DD Type de relax ('RelaxPriority' y
    'RelaxGrouping') en un mismo archivo. Rust solo tiene 'Relaxed', así que sin
    estandarizar el merge con Rust descarta todas las filas de relax (ningún
    'RelaxPriority'/'RelaxGrouping' calza con 'Relaxed') y además se graficarían
    los dos relax. Aquí descartamos el relax no elegido y renombramos el otro a
    'Relaxed'."""
    keep = RELAX_DD_TYPE[RELAX_TYPE]
    other = {'RelaxPriority', 'RelaxGrouping'} - {keep}
    df = df[~df['DD Type'].isin(other)].copy()
    df['DD Type'] = df['DD Type'].replace({keep: 'Relaxed'})
    return df

def scatter_rust_vs_other(merged_df, other_language: str, save=False, output_path=None, group_by='Problem type'):
    real_counts = merged_df[group_by].value_counts().to_dict()
    assert other_language in ['Python', 'C++'], "Solo se permite 'Python' o 'C++'"
    
    rust_col = 'Construction Time_rust'
    other_col = f"Construction Time_{'python' if other_language == 'Python' else 'cpp'}"

    min_val = 1e-6
    rust = np.clip(merged_df[rust_col], min_val, None)
    other = np.clip(merged_df[other_col], min_val, None)

    bin_resolution = 20
    merged_df[f'{other_language}_rounded'] = np.floor(np.log10(other) * bin_resolution) / bin_resolution
    merged_df['Rust_rounded'] = np.floor(np.log10(rust) * bin_resolution) / bin_resolution

    grouped = (
        merged_df
        .groupby([f'{other_language}_rounded', 'Rust_rounded', group_by])
        .size()
        .reset_index(name='count')
    )

    plt.figure(figsize=(8, 8))

    pretty_markers = ['o', 's', 'D', '^', 'v', 'P', '*', 'X', '<', '>']
    colors = plt.colormaps['tab10']
    problem_types = sorted(grouped[group_by].unique())

    marker_map = {ptype: pretty_markers[i % len(pretty_markers)] for i, ptype in enumerate(problem_types)}
    color_map = {ptype: colors(i % 10) for i, ptype in enumerate(problem_types)}

    for ptype in problem_types:
        group = grouped[grouped[group_by] == ptype]
        plt.scatter(
            10 ** group[f'{other_language}_rounded'],
            10 ** group['Rust_rounded'],
            s=20 + group['count'] * 5,
            label = f"{ptype} ({real_counts.get(ptype, 0)})",
            marker=marker_map[ptype],
            color=color_map[ptype],
            alpha=0.8,
            edgecolors='black',
            linewidths=0.5
        )

    # Escalas logarítmicas
    plt.xscale('log')
    plt.yscale('log')

    # Límites de ejes (ajustados a log)
    plt.xlim(1e-3, missing_time)
    plt.ylim(1e-3, missing_time)

    # Línea de referencia (diagonal ideal)
    plt.plot([1e-3, missing_time-10], [1e-3, missing_time-10], linestyle='--', color='gray', label=f'RUST = {other_language}')

    # Etiquetas (en inglés, sin título en la imagen)
    plt.xlabel(f'{other_language} time (s, log)')
    plt.ylabel('Rust time (s, log)')

    # Leyenda
    legend = plt.legend(loc='lower right')          
    for handle in legend.legend_handles:           
        if hasattr(handle, "set_sizes"):           
            handle.set_sizes([60])  

    plt.grid(True)

    plt.tight_layout()

    if save:
        plt.savefig(output_path)
        print(f"📊 Guardado en: {output_path}")
    else:
        plt.show()

def sync_missing_rows(df_1: pd.DataFrame, df_2: pd.DataFrame, df_1_name:str, df_2_name:str) -> tuple[pd.DataFrame, pd.DataFrame]:
    merge_cols = ['File_name', 'Max Width', 'Is Reduced', 'DD Type']

    # Dado que Rust no puede reducir, se elimina la columna 'Is Reduced' de los merges
    if df_1_name == "Rust" or df_2_name == "Rust":
        merge_cols.pop(merge_cols.index('Is Reduced'))
    base_cols = ['File_name', 'Construction Time', 'Max Width', 'Is Reduced',
                 'DD Type', 'Variables number', 'Problem type', 'Language']

    def get_missing_rows(source_df, target_df, language_name):
        # Encuentra las filas en source_df que no están en target_df según merge_cols
        merged = source_df.merge(target_df[merge_cols], on=merge_cols, how='left', indicator=True)
        missing = merged[merged['_merge'] == 'left_only']
        missing_rows = source_df.merge(missing[merge_cols], on=merge_cols, how='inner')
        missing_rows = missing_rows[base_cols].copy()
        missing_rows['Construction Time'] = missing_time
        missing_rows['Language'] = language_name
        return missing_rows

    # De df_1 a df_2
    missing_from_2 = get_missing_rows(df_1, df_2, df_1_name)
    print(f"🔍 Se encontraron {len(missing_from_2)} filas de {df_1_name} que faltan en {df_2_name}")
    df_2_updated = pd.concat([df_2, missing_from_2], ignore_index=True)

    # De df_2 a df_1
    missing_from_1 = get_missing_rows(df_2, df_1, df_2_name)
    print(f"🔍 Se encontraron {len(missing_from_1)} filas de {df_2_name} que faltan en {df_1_name}")
    df_1_updated = pd.concat([df_1, missing_from_1], ignore_index=True)

    return df_1_updated, df_2_updated

def merge_cpp_python_rust(df_cpp, df_python, df_rust) -> pd.DataFrame:
    merge_cols = ['File_name', 'Max Width', 'Is Reduced', 'DD Type']
    base_cols = ['File_name', 'Construction Time', 'Max Width', 'Is Reduced', 'DD Type', 'Variables number', 'Problem type', 'Language']

    for df in [df_cpp, df_python, df_rust]:
        df['File_name'] = df['File_name'].astype(str).apply(
            lambda x: os.path.splitext(x)[0] + '.txt'
        )

    for df in [df_cpp, df_python, df_rust]:
        df['Is Reduced'] = df['Is Reduced'].astype(str).str.lower().map({
            'true': True,
            'false': False
        }).fillna(df['Is Reduced']) 
    
    for df in [df_cpp, df_python]:
        df.loc[df['DD Type'] == 'Exact', 'Max Width'] = 99999

    df_rust['File_name'] = df_rust['File_name'].str.replace(
        r'^set_covering_', 'set_cover_', regex=True
    )
    
    df_cpp = df_cpp[base_cols]
    df_python = df_python[base_cols]
    df_rust = df_rust[base_cols]

    df_cpp, df_python = sync_missing_rows(df_cpp, df_python, "C++", "Python")
    df_cpp, df_rust = sync_missing_rows(df_cpp, df_rust,  "C++", "Rust")
    df_python, df_rust = sync_missing_rows(df_python, df_rust, "Python", "Rust")

    # Merge C++ y Python
    merged = pd.merge(
        df_cpp,
        df_python,
        on=merge_cols,
        suffixes=('_cpp', '_python')
    )

    # Merge con Rust (inner join para solo los que existen en Rust)
    merged = pd.merge(
        merged,
        df_rust[base_cols],
        on=merge_cols,
        how='inner'
    ).rename(columns={'Construction Time': 'Construction Time_rust', 'Problem type': 'Problem type_rust'})

    # Usar solo una columna 'Problem type' desde Rust o C++
    merged['Problem type'] = merged['Problem type_rust']

    return merged

### Parámetros globales que se utilizan

# Relax a graficar para C++/Python: "priority" o "grouping". Por ahora se usa
# priority. Rust no implementa grouping, así que siempre usa su CSV único.
RELAX_TYPE = "priority"
# DD Type en los CSV de C++/Python según el relax elegido. Se renombra a
# 'Relaxed' (ver standardize_relax) para que calce con el único relax de Rust.
RELAX_DD_TYPE = {"priority": "RelaxPriority", "grouping": "RelaxGrouping"}

missing_time = 310
min_variables_default = 100
# SOC Knapsack instances are inherently small (max ~36 vars); override the default threshold
min_variables_by_problem = {
    'SOC Knapsack': 0,
    'Scheduler': 0,
}
# Problem types to exclude from all plots (use normalized names, e.g. 'SOC Knapsack', 'Independent Set')
EXCLUDED_PROBLEM_TYPES = ['SOC Knapsack']
plots_folder = './plots/rust_construction_time'
archive_existing(plots_folder)
file_timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")

csv_folder    = './data'
cpp_folder    = os.path.join(csv_folder, 'cpp')
python_folder = os.path.join(csv_folder, 'python')
rust_folder   = os.path.join(csv_folder, 'rust')

try:
    print(f"🔧 Relax seleccionado (C++/Python): {RELAX_TYPE}")
    df_cpp    = load_relax(cpp_folder,    'cpp')
    df_python = load_relax(python_folder, 'python')
    df_rust   = load_latest(rust_folder,   'results_rust_*.csv')

    # Normalize problem type names before filtering so the thresholds match correctly.
    # (merge_cpp_python_rust applies the same normalization later; applying it here is harmless.)
    name_map = {
        "IndependentSet": "Independent Set",
        "Set Covering": "Set Cover",
        "SOCKnapsack": "SOC Knapsack",
    }
    for df in [df_cpp, df_python, df_rust]:
        df['Problem type'] = df['Problem type'].replace(name_map)

    if EXCLUDED_PROBLEM_TYPES:
        for df in [df_cpp, df_python, df_rust]:
            df.drop(df[df['Problem type'].isin(EXCLUDED_PROBLEM_TYPES)].index, inplace=True)

    def filter_min_vars(df):
        thresholds = df['Problem type'].map(
            lambda p: min_variables_by_problem.get(p, min_variables_default)
        )
        return df[df['Variables number'] >= thresholds]

    df_rust   = filter_min_vars(df_rust)
    df_cpp    = filter_min_vars(df_cpp)
    df_python = filter_min_vars(df_python)


    df_cpp["Language"] = "C++"
    df_python["Language"] = "Python"
    df_rust["Language"] = "Rust"

    # Deja un solo relax y lo renombra a 'Relaxed' para que calce con Rust.
    df_cpp    = standardize_relax(df_cpp)
    df_python = standardize_relax(df_python)

    dd_types = df_cpp['DD Type'].unique()

    df_merged = merge_cpp_python_rust(df_cpp, df_python, df_rust)

    ### 3 plots principales por DD Type ###
    for dd_type in dd_types:
        filtered_df = df_merged[
            (df_merged['DD Type'] == dd_type)
        ]

        if filtered_df.empty:
            print(f"⏭️ No hay datos para DD Type: {dd_type}")
            continue

        #Asegurar la columna para graficar por tipo de problema
        filtered_df = filtered_df.copy()
        filtered_df['Problem type'] = filtered_df.get('Problem type_rust', 'Unknown')
        filtered_df = filtered_df.drop(columns=[
            col for col in ['Problem type_cpp', 'Problem type_python', 'Problem type_rust']
            if col in filtered_df.columns
        ])

        scatter_rust_vs_other(
            filtered_df,
            save=True,
            other_language='C++',
            output_path=f'{plots_folder}/comparison_rust_cpp_{dd_type.lower().replace(" ", "_")}_{file_timestamp}.png'
        )

        scatter_rust_vs_other(
            filtered_df,
            save=True,
            other_language='Python',
            output_path=f'{plots_folder}/comparison_rust_python_{dd_type.lower().replace(" ", "_")}_{file_timestamp}.png'
        )


except Exception as e:
    print(f"Error leyendo: {e}")

