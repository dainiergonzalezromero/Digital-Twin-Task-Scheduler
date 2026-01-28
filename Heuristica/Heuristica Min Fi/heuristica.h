#ifndef HEURISTICA_H
#define HEURISTICA_H

#include <vector>
#include "datos.h"

struct Resultado {
    std::vector<int> servidor;   // servidor asignado a cada tarea
    std::vector<int> s;          // tiempo inicio
    std::vector<int> f;          // tiempo fin
    bool factible;
};


Resultado heuristicaGreedy(
    const std::vector<Procesador>& procesadores,
    const std::vector<Tarea>& tareas,
    const std::vector<Precedencia>& precedencias,
    const std::vector<Comunicacion>& comunicaciones
);

#endif
