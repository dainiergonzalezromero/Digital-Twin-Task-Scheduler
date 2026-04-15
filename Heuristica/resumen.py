
import os

def procesar_archivo(nombre_archivo):
    h1_tiempo = []
    h1_f = []
    h1_costo = []

    h2_tiempo = []
    h2_f = []
    h2_costo = []

    with open(nombre_archivo, "r") as f:
        lineas = f.readlines()

    i = 0
    while i < len(lineas):
        linea = lineas[i]

        if "Heurística 1" in linea:
            tiempo = float(lineas[i+1].split(":")[1].replace("ms",""))
            suma_f = int(lineas[i+2].split(":")[1])
            suma_costo = int(lineas[i+3].split(":")[1])

            h1_tiempo.append(tiempo)
            h1_f.append(suma_f)
            h1_costo.append(suma_costo)

            i += 4
            continue

        if "Heurística 2" in linea:
            tiempo = float(lineas[i+1].split(":")[1].replace("ms",""))
            suma_f = int(lineas[i+2].split(":")[1])
            suma_costo = int(lineas[i+3].split(":")[1])

            h2_tiempo.append(tiempo)
            h2_f.append(suma_f)
            h2_costo.append(suma_costo)

            i += 4
            continue

        i += 1

    resumen = {
        "h1_tiempo": sum(h1_tiempo) / len(h1_tiempo),
        "h1_f": sum(h1_f) / len(h1_f),
        "h1_costo": sum(h1_costo) / len(h1_costo),
        "h2_tiempo": sum(h2_tiempo) / len(h2_tiempo),
        "h2_f": sum(h2_f) / len(h2_f),
        "h2_costo": sum(h2_costo) / len(h2_costo),
    }

    return resumen


# Buscar todos los txt de resultados
txt_files = [f for f in os.listdir() if f.endswith(".txt") and f != "resumen_final.txt"]

# Crear archivo resumen final
with open("resumen_final.txt", "w") as out:

    for txt in sorted(txt_files, key=lambda x: int(x.split(".")[0])):
        resumen = procesar_archivo(txt)

        out.write(f"Archivo: {txt}\n\n")

        out.write("Heurística 1\n")
        out.write(f"Tiempo promedio: {resumen['h1_tiempo']:.3f} ms\n")
        out.write(f"Suma f promedio: {resumen['h1_f']:.0f}\n")
        out.write(f"Suma costo promedio: {resumen['h1_costo']:.0f}\n\n")

        out.write("Heurística 2\n")
        out.write(f"Tiempo promedio: {resumen['h2_tiempo']:.3f} ms\n")
        out.write(f"Suma f promedio: {resumen['h2_f']:.0f}\n")
        out.write(f"Suma costo promedio: {resumen['h2_costo']:.0f}\n")

        out.write("\n---------------------------------------\n\n")