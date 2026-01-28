#ifndef HEURISTICA_H
#define HEURISTICA_H

#include <vector>
#include "datos.h"

struct Resultado {
    std::vector<int> servidor;   // servidor asignado a cada tarea
    std::vector<int> s;          // tiempo inicio
    std::vector<int> f;          // tiempo fin
    std::vector<int> cost;       // costo por unidad de tiempo del procesador asignado
    int total_delays;         // suma total de todos los delays de comunicación
    bool factible;
};


Resultado heuristicaGreedy(
    const std::vector<Procesador>& procesadores,
    const std::vector<Tarea>& tareas,
    const std::vector<Precedencia>& precedencias,
    const std::vector<Comunicacion>& comunicaciones,
    const double alfa,
    const double beta,
    const double gamma, 
    bool debug
);

#endif