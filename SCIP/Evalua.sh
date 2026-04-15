#!/bin/bash

echo "Simulacion DT instancias 4 metodos"

echo "10 tareas"
cp dats/datos.dat instancia.dat

# Generar el modelo LP
zimpl -o prueba modelo.zpl

# Ejecutar CPLEX y guardar salida (redirigir a /dev/null para no mostrar)
cplex < exacto > /dev/null 2>&1

# Formatear los resultados desde resultado.sol
python3 formatear_resultados.py instancia.dat