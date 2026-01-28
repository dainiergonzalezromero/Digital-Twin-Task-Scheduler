#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>

#include "datos.h"
#include "lector.h"
#include "visualizador.h"
#include "heuristica.h"

using namespace std;



int main(int argc, char* argv[]) {

    if (argc < 2) {
        cerr << "Uso: " << argv[0] << " <archivo_instancia>\n";
        return 1;
    }

    vector<Procesador> procesadores;
    vector<Tarea> tareas;
    vector<Precedencia> precedencias;
    vector<Comunicacion> comunicaciones;

    if (!leerDatos(argv[1], procesadores, tareas, precedencias, comunicaciones)) {
        cerr << "Error al leer archivo\n";
        return 1;
    }

    mostrarResumenGeneral(
        procesadores, tareas, precedencias, comunicaciones, argv[1]
    );

    auto resultado = heuristicaGreedy(
        procesadores, tareas, precedencias, comunicaciones
    );

    if (!resultado.factible) {
        cout << "\n❌ Solución NO factible\n";
        return 0;
    }
  

    
    int objetive_function = 0;
    cout << "\n✅ SOLUCIÓN HEURÍSTICA\n";
    for (size_t i = 0; i < tareas.size(); i++) {
        cout << "T" << i
             << " -> S" << resultado.servidor[i]
             << " | s=" << resultado.s[i]
             << " f=" << resultado.f[i] << "\n";
        objetive_function += resultado.f[i];
    }

    cout << "Funcion Objetivo: "<< objetive_function << endl;

    return 0;
}
