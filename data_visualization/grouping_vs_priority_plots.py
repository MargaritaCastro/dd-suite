"""Compara el relax por GROUPING (DD Type 'RelaxGrouping') contra el relax por
PRIORITY (DD Type 'Relaxed'). Ambos viven ahora en el mismo archivo
results_<lang>_*.csv y se distinguen por la columna 'DD Type'.

Por cada lenguaje (Python y C++) genera 2 scatter plots:
  1) Tiempo de construcción: grouping vs priority (color por Problem type).
  2) Cotas como % de gap respecto al óptimo del DD Exact (color por Max Width).

En ambos cada punto es una instancia (File_name + Max Width). La diagonal
punteada marca grouping == priority: puntos por debajo => grouping mejor (menos
tiempo / gap más chico), por encima => peor.
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
    """Crea 'folder' y su 'deprecated/' y mueve ahí las salidas ya existentes
    antes de generar nuevas, para no acumular versiones viejas en la raíz."""
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
PRIORITY_TYPE = "RelaxPriority"  # relax por prioridad (descarte/merge por prioridad)
GROUPING_TYPE = "RelaxGrouping"  # relax por agrupamiento
EXACT_TYPE    = "Exact"          # DD exacto -> óptimo de referencia para el gap


# ── Helpers de carga ────────────────────────────────────────────────────────────

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
    print(f"  {os.path.basename(path)}  ({len(df)} filas)")
    return df


_PROBLEM_TYPE_MAP = {"IndependentSet": "Independent Set", "Set Covering": "Set Cover"}

def norm_problem_type(s):
    return _PROBLEM_TYPE_MAP.get(str(s).strip(), str(s).strip())

def norm_key(s):
    """Quita extensiones conocidas y unifica set_covering_ -> set_cover_."""
    s = str(s).strip()
    for ext in (".txt", ".clq", ".col"):
        if s.endswith(ext):
            s = s[: -len(ext)]
            break
    return s.replace("set_covering_", "set_cover_")


# ── Referencia de óptimos (Gurobi) ──────────────────────────────────────────────
# Gurobi y el DD Exact dan el mismo óptimo; se combinan para maximizar cobertura.

print("Cargando referencia Gurobi:")
df_gpy = load_latest(PY_DIR, "results_gurobi_python_*.csv")
if df_gpy is None:
    GUROBI_REF = {}
    print("  (sin archivo Gurobi; se usará sólo el Exact)")
else:
    df_gpy["File_key"]     = df_gpy["File_name"].apply(norm_key)
    df_gpy["Problem type"] = df_gpy["Problem type"].apply(norm_problem_type)
    df_gpy["Gurobi_Sol"]   = pd.to_numeric(df_gpy["Best solution"], errors="coerce")
    GUROBI_REF = (df_gpy.dropna(subset=["Gurobi_Sol"])
                  .groupby(["File_key", "Problem type"])["Gurobi_Sol"].mean().to_dict())
    print(f"  Referencia Gurobi: {len(GUROBI_REF)} instancias")


# ── Construcción del DataFrame comparado por lenguaje ───────────────────────────

def build_comparison(folder, prefix):
    """Devuelve un DataFrame con una fila por (instancia, Max Width) que tiene
    columnas *_priority y *_grouping de Construction Time y Gap %.

    Priority (DD Type 'RelaxPriority') y grouping (DD Type 'RelaxGrouping') viven
    ahora en el mismo archivo results_<prefix>_*.csv y se separan por 'DD Type'."""
    df = load_latest(folder, f"results_{prefix}_*.csv")

    if df is None:
        print(f"  ⏭️  No hay archivo de resultados para {prefix}")
        return None

    def prep(dd_type):
        sub = df[(df["DD Type"] == dd_type) & (df["Max Width"].isin(WIDTHS))].copy()
        sub["File_key"]     = sub["File_name"].apply(norm_key)
        sub["Problem type"] = sub["Problem type"].apply(norm_problem_type)
        # Promedia repeticiones de la misma (instancia, width) si las hubiera
        return (sub.groupby(["File_key", "Problem type", "Max Width"], as_index=False)
                   .agg({"Construction Time": "mean", "Optimization Value": "mean"}))

    prio = prep(PRIORITY_TYPE)
    grp  = prep(GROUPING_TYPE)

    if prio.empty or grp.empty:
        print(f"  ⏭️  Sin filas {PRIORITY_TYPE}/{GROUPING_TYPE} para {prefix}")
        return None

    # Referencia de óptimos: valor del DD Exact del mismo lenguaje (independiente
    # del width) combinado con Gurobi (dan el mismo óptimo). Se prioriza el Exact
    # y se rellena con Gurobi lo que falte. Sin referencia => gap NaN y se descarta.
    exact = df[df["DD Type"] == EXACT_TYPE].copy()
    exact["File_key"]     = exact["File_name"].apply(norm_key)
    exact["Problem type"] = exact["Problem type"].apply(norm_problem_type)
    exact_ref = (exact.groupby(["File_key", "Problem type"])["Optimization Value"]
                 .mean().to_dict())
    ref = {**GUROBI_REF, **exact_ref}   # Exact pisa a Gurobi donde ambos existen
    print(f"  Referencia óptimos: {len(ref)} instancias "
          f"(Exact {len(exact_ref)} + Gurobi {len(GUROBI_REF)})")

    merged = prio.merge(grp, on=["File_key", "Problem type", "Max Width"],
                        suffixes=("_priority", "_grouping"))
    if merged.empty:
        print(f"  ⏭️  El merge priority/grouping quedó vacío para {prefix}")
        return None

    # Gap en valor absoluto respecto al óptimo: |cota - opt| / |opt|. Siempre >= 0
    # para que el gráfico viva en el cuadrante 1.
    def gap_pct(row, col):
        opt = ref.get((row["File_key"], row["Problem type"]), np.nan)
        val = row[col]
        if pd.isna(opt) or pd.isna(val) or opt == 0:
            return np.nan
        return abs(val - opt) / abs(opt) * 100.0

    merged["Gap_priority"] = merged.apply(lambda r: gap_pct(r, "Optimization Value_priority"), axis=1)
    merged["Gap_grouping"] = merged.apply(lambda r: gap_pct(r, "Optimization Value_grouping"), axis=1)

    print(f"  ✅ {prefix}: {len(merged)} pares (instancia, width) comparados")
    return merged


# ── Scatter genérico coloreado por una categoría ────────────────────────────────

WIDTH_CMAP  = plt.colormaps["viridis"]
WIDTH_COLOR = {w: WIDTH_CMAP(i / max(1, len(WIDTHS) - 1)) for i, w in enumerate(WIDTHS)}
PROB_CMAP   = plt.colormaps["tab10"]

def scatter_compare(df, x_col, y_col, color_col, title, xlabel, ylabel,
                    output_path, log_scale=False, floor=None, label_prefix=""):
    plt.figure(figsize=(8, 8))

    data = df.dropna(subset=[x_col, y_col]).copy()
    if data.empty:
        print(f"  ⏭️  Sin datos para '{title}'")
        plt.close()
        return

    x = data[x_col].to_numpy(dtype=float)
    y = data[y_col].to_numpy(dtype=float)
    if floor is not None:                       # evita ceros/negativos en escala log
        x = np.clip(x, floor, None)
        y = np.clip(y, floor, None)

    # Orden y paleta fijos según la columna de color
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

    # Diagonal de referencia priority == grouping
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
    print(f"  💾 Guardado: {output_path}")


# ── Gap medio vs width (una línea por tipo de relax) ────────────────────────────

RELAX_COLORS = {"priority": "#1f77b4", "grouping": "#d62728"}
IQR_WHISKER  = 1.5   # regla de Tukey: descartar fuera de [Q1−1.5·IQR, Q3+1.5·IQR]

def _iqr_filter(vals):
    """Quita outliers con la regla del IQR (Tukey) y devuelve la serie filtrada
    junto con cuántos se descartaron. Si no quedara nada, conserva el original."""
    vals = vals.dropna()
    if len(vals) < 4:                       # muy pocos datos para definir outliers
        return vals, 0
    q1, q3 = vals.quantile(0.25), vals.quantile(0.75)
    iqr = q3 - q1
    lo, hi = q1 - IQR_WHISKER * iqr, q3 + IQR_WHISKER * iqr
    kept = vals[(vals >= lo) & (vals <= hi)]
    if kept.empty:
        return vals, 0
    return kept, len(vals) - len(kept)

def gap_vs_width(merged, lang_label, prefix):
    """Un archivo por example: gap% vs Max Width con una línea por tipo de relax
    (priority/grouping). En cada width se descartan los outliers con la regla del
    IQR; la línea es el promedio de los datos restantes y la banda su p25–p75."""
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
        print(f"  💾 Guardado: {output_path}")


# ── Generación ──────────────────────────────────────────────────────────────────

LANGS = [("python", "Python", PY_DIR), ("cpp", "C++", CPP_DIR)]

for prefix, lang_label, folder in LANGS:
    print(f"\n=== {lang_label} ===")
    merged = build_comparison(folder, prefix)
    if merged is None:
        continue

    # 1) Tiempo de construcción (log-log; el grouping puede ser muy chico)
    #    color por Problem type
    scatter_compare(
        merged,
        x_col="Construction Time_priority",
        y_col="Construction Time_grouping",
        color_col="Problem type",
        title=f"{lang_label} — Tiempo de construcción: grouping vs priority\n"
              "(bajo la diagonal => grouping más rápido)",
        xlabel="Construction Time priority (s)",
        ylabel="Construction Time grouping (s)",
        output_path=os.path.join(PLOTS_DIR, f"grouping_vs_priority_time_{prefix}_{ts}.png"),
        log_scale=True,
        floor=1e-6,
    )

    # 2) Gap de la cota (%) vs Max Width, una línea por tipo de relax.
    #    Un archivo por cada example (Problem type).
    gap_vs_width(merged, lang_label, prefix)

print("\nListo!")
