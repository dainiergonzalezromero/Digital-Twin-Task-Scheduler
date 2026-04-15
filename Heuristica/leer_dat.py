# ==================== IMPORTS ====================
import pandas as pd
import tkinter as tk
from tkinter import filedialog
import networkx as nx
from networkx.algorithms.simple_paths import all_simple_paths


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
        {"ID": int(l[0]), "RAM": float(l[1]), "CPU": float(l[2]), "COST": float(l[3])}
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


# ==================== MAIN ====================
file_path = seleccionar_archivo()
if not file_path:
    print("Selección cancelada.")
    exit()

try:
    proc, tareas, prec, proc_prec = leer_datos(file_path)
    Processors, Tasks = construir_dataframes(proc, tareas, prec, proc_prec)

except Exception as e:
    print(f"❌ Error: {e}")


Mist_Tasks = Tasks[Tasks["L"] != 0]
ID_Actuator_Tasks = Mist_Tasks[Mist_Tasks["Successor Task"].apply(len) == 0].index.tolist()
ID_Sensor_Tasks = Mist_Tasks[Mist_Tasks["Successor Task"].apply(len) != 0].index.tolist()


tareas = len(Tasks.index.tolist())


# Crear grafo dirigido
G = nx.DiGraph()

# Agregar aristas basadas en tus datos
for idx, row in Tasks.iterrows():
    for succ in row['Successor Task']:
        G.add_edge(idx, succ)

max_camino = dict()
for sensor in ID_Sensor_Tasks:
    for actuator in ID_Actuator_Tasks:
        try:
            caminos = list(all_simple_paths(G, sensor, actuator))
            for camino in caminos:
                max_camino[len(camino)] = camino
        except nx.NetworkXNoPath:
            continue

print(f"Longitud máxima del camino: {max_camino}")
