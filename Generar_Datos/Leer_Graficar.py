# ==================== BACKEND OBLIGATORIO ====================
import matplotlib
matplotlib.use("TkAgg")

# ==================== IMPORTS ====================
import pandas as pd
import tkinter as tk
from tkinter import filedialog
import networkx as nx
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle

# ==================== SELECCIÓN DE ARCHIVO ====================
def seleccionar_archivo():
    root = tk.Tk()
    root.withdraw()
    file_path = filedialog.askopenfilename(
        title="Selecciona un archivo",
        filetypes=[("Archivos de Datos", "*.dat"), ("Todos los archivos", "*.*")]
    )
    root.destroy()
    return file_path

# ==================== LECTURA DE DATOS ====================
def leer_datos(file_path):
    with open(file_path, "r") as f:
        lineas = [l.strip().split("\t") for l in f if l.strip()]

    it = iter(lineas)

    # Procesadores (NO se usan para graficar)
    P = int(next(it)[0])
    procesadores = [
        {"ID": int(l[0]), "RAM": float(l[1]), "CPU": float(l[2])}
        for l in (next(it) for _ in range(P))
    ]

    # Tareas
    τ = int(next(it)[0]) + 1
    tareas = [
        {
            "ID": int(l[0]),
            "C": float(l[1]),
            "T": float(l[2]),
            "D": float(l[3]),
            "r": float(l[4]),
            "M": float(l[5]),
            "L": float(l[6]),
        }
        for l in (next(it) for _ in range(τ))
    ]

    # Precedencias tareas
    TP = int(next(it)[0])
    precedencias = [
        {"ID": int(l[0]), "Succesor Task": int(l[1])}
        for l in (next(it) for _ in range(TP))
        if int(l[2]) != 0
    ]

    # Precedencias procesadores (NO se usan para graficar)
    CP = int(next(it)[0])
    proc_precedencias = [
        {
            "ID": int(l[0]),
            "Succesor Processor": int(l[1]),
            "Delay": int(l[2]),
        }
        for l in (
            line for line in (next(it) for _ in range(CP))
            if int(line[2]) < 1000 and int(line[2]) != 0
        )
    ]

    return procesadores, tareas, precedencias, proc_precedencias

# ==================== DATAFRAMES ====================
def construir_dataframes(proc, tareas, prec, proc_prec):
    Processors = pd.DataFrame(proc).set_index("ID")
    Tasks = pd.DataFrame(tareas).set_index("ID")

    Precedences = pd.DataFrame(prec)

    succ_tasks = Precedences.groupby("ID")["Succesor Task"].apply(list).to_dict()
    Tasks["Successor Task"] = Tasks.index.map(lambda i: succ_tasks.get(i, []))
    Tasks["Successor Task"] = Tasks["Successor Task"].apply(
        lambda x: x if isinstance(x, list) else []
    )

    return Processors, Tasks

# ==================== LAYOUT JERÁRQUICO CORRECTO ====================
def layout_jerarquico(G, sep_x=4.0, sep_y=3.5):
    # Nivel = distancia máxima desde una raíz
    levels = {}
    for node in nx.topological_sort(G):
        preds = list(G.predecessors(node))
        if not preds:
            levels[node] = 0
        else:
            levels[node] = max(levels[p] for p in preds) + 1

    # Agrupar nodos por nivel
    level_nodes = {}
    for n, lvl in levels.items():
        level_nodes.setdefault(lvl, []).append(n)

    # Posiciones
    pos = {}
    for lvl, nodes in level_nodes.items():
        nodes = sorted(nodes)
        width = (len(nodes) - 1) * sep_x
        x0 = -width / 2   # 👈 centra cada nivel
        for i, n in enumerate(nodes):
            pos[n] = (x0 + i * sep_x, -lvl * sep_y)

    return pos

def color_procesador(pid):
    if pid == 0:
        return "#E0E0E0"  # Mist
    colores = [
        "#FFE082", "#FFCCBC", "#C5E1A5",
        "#B3E5FC", "#D1C4E9", "#FFECB3"
    ]
    return colores[(int(pid) - 1) % len(colores)]


# ==================== GRAFO DE TAREAS ====================
def graficar_tareas_con_procesador(Tasks):
    G = nx.DiGraph()
    node_info = {}

    # ----- NODOS -----
    for tid, row in Tasks.iterrows():
        n = f"T{tid}"
        G.add_node(n)
        node_info[n] = row.to_dict()

    # ----- ARISTAS -----
    for tid, row in Tasks.iterrows():
        for succ in row["Successor Task"]:
            G.add_edge(f"T{tid}", f"T{succ}")

    # ----- POSICIONES -----
    pos = layout_jerarquico(G)

    fig, ax = plt.subplots(
        figsize=(max(16, len(G.nodes) * 0.9), 10)
    )

    # ----- RECTÁNGULOS DE PROCESADOR (FONDO) -----
    rect_w, rect_h = 1.6, 1.0

    for node, (x, y) in pos.items():
        tid = int(node[1:])
        L = Tasks.loc[tid, "L"]
        color = color_procesador(L)

        etiqueta = f"P{int(L)}" if L != 0 else "PX"

        rect = Rectangle(
            (x - rect_w / 2, y - rect_h / 2),
            rect_w,
            rect_h,
            facecolor=color,
            edgecolor="black",
            linewidth=1
        )
        ax.add_patch(rect)

        ax.text(
            x,
            y - 0.65,
            etiqueta,
            ha="center",
            va="top",
            fontsize=8
        )

    # ----- ARISTAS -----
    nx.draw_networkx_edges(
        G, pos,
        arrowstyle="-|>",
        arrowsize=18,
        width=1.4,
        connectionstyle="arc3,rad=0.15",
        min_source_margin=20,
        min_target_margin=22,
        ax=ax
    )

    # ----- NODOS (PRIMER PLANO) -----
    nx.draw_networkx_nodes(
        G, pos,
        node_size=800,
        node_color="skyblue",
        node_shape="o",
        ax=ax
    )

    nx.draw_networkx_labels(G, pos, ax=ax)

    # ----- HOVER -----
    annot = ax.annotate(
        "", xy=(0, 0), xytext=(15, 15),
        textcoords="offset points",
        bbox=dict(boxstyle="round", fc="w"),
        arrowprops=dict(arrowstyle="->")
    )
    annot.set_visible(False)

    def update_annot(node):
        x, y = pos[node]
        annot.xy = (x, y)
        text = f"{node}\n"
        for k, v in node_info[node].items():
            text += f"{k}: {v}\n"
        annot.set_text(text)

    def hover(event):
        if event.inaxes == ax and event.xdata and event.ydata:
            for node, (x, y) in pos.items():
                if abs(event.xdata - x) < 0.4 and abs(event.ydata - y) < 0.4:
                    update_annot(node)
                    annot.set_visible(True)
                    fig.canvas.draw_idle()
                    return
        annot.set_visible(False)
        fig.canvas.draw_idle()

    fig.canvas.mpl_connect("motion_notify_event", hover)

    ax.set_title("Tareas con procesador asignado (L)")
    ax.axis("off")
    plt.show()

# ==================== MAIN ====================
file_path = seleccionar_archivo()
if not file_path:
    print("Selección cancelada.")
    exit()

try:
    proc, tareas, prec, proc_prec = leer_datos(file_path)
    _, Tasks = construir_dataframes(proc, tareas, prec, proc_prec)

    graficar_tareas_con_procesador(Tasks)

except Exception as e:
    print(f"❌ Error: {e}")
