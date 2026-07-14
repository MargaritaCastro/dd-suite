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
    """Carga el CSV del relax elegido en RELAX_TYPE.

    - "priority" -> results_<lang>_*.csv (excluyendo los *_grouping_*)
    - "grouping" -> results_<lang>_grouping_*.csv
    """
    if RELAX_TYPE == "grouping":
        return load_latest(folder, f'results_{lang}_grouping_*.csv')
    return load_latest(folder, f'results_{lang}_*.csv', exclude_grouping=True)

def sync_missing_rows(df_cpp: pd.DataFrame, df_python: pd.DataFrame):
    join_keys = ['File_name', 'Max Width', 'Is Reduced', 'DD Type', 'Variables number']

    # Detectar qué filas están en C++ pero no en Python
    cpp_missing_in_python = df_cpp.merge(
        df_python[join_keys], on=join_keys, how='left', indicator=True
    ).query('_merge == "left_only"').drop(columns=['_merge'])

    print(f"📥 Agregando {len(cpp_missing_in_python)} filas desde C++ a Python")

    # Completar datos faltantes para Python
    cpp_missing_in_python = cpp_missing_in_python.copy()
    cpp_missing_in_python['Construction Time'] = missing_time
    cpp_missing_in_python['Timeout'] = True

    # Añadir al DataFrame Python
    df_python = pd.concat([df_python, cpp_missing_in_python], ignore_index=True)

    # Detectar qué filas están en Python pero no en C++
    python_missing_in_cpp = df_python.merge(
        df_cpp[join_keys], on=join_keys, how='left', indicator=True
    ).query('_merge == "left_only"').drop(columns=['_merge'])

    print(f"📥 Agregando {len(python_missing_in_cpp)} filas desde Python a C++")

    # Completar datos faltantes para C++
    python_missing_in_cpp = python_missing_in_cpp.copy()
    python_missing_in_cpp['Construction Time'] = missing_time
    python_missing_in_cpp['Timeout'] = True

    # Añadir al DataFrame C++
    df_cpp = pd.concat([df_cpp, python_missing_in_cpp], ignore_index=True)

    return df_cpp, df_python

def scatter_plot(merged_df, title, save=False, output_path=None, group_by='Problem type'):
    real_counts = merged_df[group_by].value_counts().to_dict()
    plt.figure(figsize=(8, 8))

    min_val = 1e-6
    cpp = np.clip(merged_df['Construction Time_cpp'], min_val, None)
    py = np.clip(merged_df['Construction Time_python'], min_val, None)

    # Se agrupan los valores de Construction Time en bins logarítmicos
    bin_resolution = 20  # usra 10 para 0.1, 20 para 0.05, etc.
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

    # Crear mapa de marcadores y colores (colores únicos para cada tipo)
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

    # Escalas logarítmicas
    plt.xscale('log')
    plt.yscale('log')

    # Límites de ejes (ajustados a log)
    plt.xlim(1e-3, missing_time)
    plt.ylim(1e-3, missing_time)

    # Línea de referencia (diagonal ideal)
    plt.plot([1e-3, missing_time-10], [1e-3, missing_time-10], linestyle='--', color='gray', label='c++ = python')

    # Etiquetas (en inglés, sin título en la imagen)
    plt.xlabel('C++ time (s, log)')
    plt.ylabel('Python time (s, log)')

    # Leyenda
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

### Parámetros globales que se utilizan

# Relax a graficar: "priority" o "grouping". Por ahora se usa priority.
RELAX_TYPE = "priority"

missing_time = 310
min_variables_default = 100
# SOC Knapsack instances are inherently small (max ~36 vars); override the default threshold
min_variables_by_problem = {
    'SOC Knapsack': 0,
    'Scheduler': 0
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
    # 1) Cargar CSV y sincronizar filas faltantes
    # ---------------------------------------------------------------------------
    print(f"🔧 Relax seleccionado: {RELAX_TYPE}")
    df_cpp_without_timeout    = load_relax(cpp_folder,    'cpp')
    df_python_without_timeout = load_relax(python_folder, 'python')

    dd_types = df_cpp_without_timeout['DD Type'].unique() # los tipos presentes

    # Añadir filas faltantes entre C++ y Python
    df_cpp, df_python = sync_missing_rows(df_cpp_without_timeout, df_python_without_timeout)

    if EXCLUDED_PROBLEM_TYPES:
        for df in [df_cpp, df_python]:
            df.drop(df[df['Problem type'].isin(EXCLUDED_PROBLEM_TYPES)].index, inplace=True)

    # ---------------------------------------------------------------------------
    # 2) Filtrado inicial por tamaño de instancia (umbral por tipo de problema)
    # ---------------------------------------------------------------------------
    def filter_min_vars(df):
        thresholds = df['Problem type'].map(
            lambda p: min_variables_by_problem.get(p, min_variables_default)
        )
        return df[df['Variables number'] >= thresholds]

    df_cpp    = filter_min_vars(df_cpp)
    df_python = filter_min_vars(df_python)


    if df_cpp.empty or df_python.empty:
        raise ValueError("❌ No quedan datos tras filtrar por Variables number >= 100")


    # ---------------------------------------------------------------------------
    # 3) Merge único (ya filtrado)
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
        raise ValueError("❌ El merge resultó vacío; revisa los filtros y las claves de unión.")

    # Asegurar columna unificada de tipo de problema
    merged_df['Problem type'] = merged_df['Problem type_cpp']
    merged_df = merged_df.drop(columns=['Problem type_cpp', 'Problem type_python'])

    # ---------------------------------------------------------------------------
    # 4) Graficar por cada DD Type (3 plots principales)
    # ---------------------------------------------------------------------------
    # Nombre amigable para los titulos de los plots (no afecta los nombres de archivo)
    DD_LABEL = {"RelaxPriority": "Relax", "RelaxGrouping": "Relax (grouping)"}

    for dd_type in dd_types:
        sub_df = merged_df[merged_df['DD Type'] == dd_type].copy()

        if sub_df.empty:
            print(f"⏭️  No hay datos para DD Type: {dd_type} con Variables number >= {min_variables_default}")
            continue

        output_path = f'{plots_folder}/construction_time_{dd_type.lower().replace(" ", "_")}_{file_timestamp}.png'
        print(f"📊 Generando gráfico para DD Type: {dd_type}")

        scatter_plot(
            sub_df,
            title=f'Comparación de Construction Time - DD Type: {DD_LABEL.get(dd_type, dd_type)}',
            save=True,
            output_path=output_path
        )

    # ---------------------------------------------------------------------------
    # 5) Graficar para Relaxed y Restricted por width (6 plots secundarios)
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
                print(f"⏭️  No hay datos para DD Type: {dd_type} y Problem type: {problem_type}")
                continue

            out_name = f"construction_time_{dd_type.lower().replace(' ', '_')}_{problem_type.lower().replace(' ', '_')}_{file_timestamp}.png"
            output_path = f"{plots_folder}/{out_name}"

            print(f"📊 Generando gráfico para DD Type: {dd_type} y Problem type: {problem_type}")

            scatter_plot(
                sub_df,
                title=f"Comparación de Construction Time\nDD Type: {DD_LABEL.get(dd_type, dd_type)} • Problem type: {problem_type}",
                save=True,
                output_path=output_path,
                group_by='Max Width'          # ← la leyenda mostrará los distintos widths
            )

except Exception as e:
    print(f"Error leyendo: {e}")

