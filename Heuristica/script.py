import os
import subprocess

dir_heutistic_1 = "Heuristica_Min_Fi_Delta_Costo"
dir_heutistic_2 = "Heuristica_Min_Fi_Delta_Costo_2"
dir_dats = "dats"

alpha = 1
beta = 0
gamma = 10

dats_files = os.listdir(dir_dats)
outputs = {}

# Agrupar archivos por cantidad de tareas
for dat_file in dats_files:
    if dat_file.endswith(".dat"):
        # Corregir: extraer solo el número
        key = dat_file.split("_")[0][:-3] # Eliminar "dat" al final
        outputs.setdefault(key, []).append(os.path.join(dir_dats, dat_file))

for key, paths in sorted(outputs.items()):
    print(f"Procesando {key} tareas...")  # Debug
    
    try:
        with open(f"{key}.txt", "w") as f:
            # Parámetros del experimento
            f.write(f"Alpha={alpha} Beta={beta} Gamma={gamma}\n")
            f.write(f"Tareas = {key}\n")
            f.write("----------------------------------\n\n")

            for dat_path in paths:
                dat_file = os.path.basename(dat_path)
                print(f"  Procesando {dat_file}")  # Debug

                f.write(f"Heurística 1 -> {dat_file}\n")
                
                try:
                    result1 = subprocess.run(
                        [f"./{dir_heutistic_1}/scheduler",
                         dat_path,
                         f"--alpha={alpha}",
                         f"--beta={beta}",
                         f"--gamma={gamma}",
                         "--t"],
                        capture_output=True,
                        text=True,
                        check=True  # Lanza excepción si el comando falla
                    )
                    f.write(result1.stdout.strip() + "\n")
                    if result1.stderr:
                        print(f"    Error en H1: {result1.stderr}")  # Debug
                        f.write(f"ERROR: {result1.stderr}\n")
                except subprocess.CalledProcessError as e:
                    error_msg = f"Error ejecutando H1: {e}\nSTDERR: {e.stderr}"
                    print(error_msg)  # Debug
                    f.write(f"ERROR: {error_msg}\n")
                except Exception as e:
                    error_msg = f"Excepción en H1: {e}"
                    print(error_msg)
                    f.write(f"ERROR: {error_msg}\n")

                f.write(f"Heurística 2 -> {dat_file}\n")
                
                try:
                    result2 = subprocess.run(
                        [f"./{dir_heutistic_2}/scheduler",
                         dat_path,
                         f"--alpha={alpha}",
                         f"--beta={beta}",
                         f"--gamma={gamma}",
                         "--t"],
                        capture_output=True,
                        text=True,
                        check=True
                    )
                    f.write(result2.stdout.strip() + "\n\n")
                    if result2.stderr:
                        print(f"    Error en H2: {result2.stderr}")
                        f.write(f"ERROR: {result2.stderr}\n")
                except subprocess.CalledProcessError as e:
                    error_msg = f"Error ejecutando H2: {e}\nSTDERR: {e.stderr}"
                    print(error_msg)
                    f.write(f"ERROR: {error_msg}\n")
                except Exception as e:
                    error_msg = f"Excepción en H2: {e}"
                    print(error_msg)
                    f.write(f"ERROR: {error_msg}\n")
                    
    except Exception as e:
        print(f"Error procesando {key}: {e}")