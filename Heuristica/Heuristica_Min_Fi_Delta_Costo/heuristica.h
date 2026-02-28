#ifndef HEURISTICA_H
#define HEURISTICA_H

#include <vector>
#include <limits>  // Para numeric_limits
#include "datos.h"

struct Resultado {
    std::vector<int> servidor;   // servidor asignado a cada tarea
    std::vector<int> s;          // tiempo inicio
    std::vector<int> f;          // tiempo fin
    std::vector<int> cost;       // costo por unidad de tiempo del procesador asignado
    double total_delays;         // suma total de todos los delays de comunicación (cambiado a double)
    double tiempo_ejecucion_ms;
    bool factible;
};

// Declaración de la función principal con reintentos
Resultado heuristicaGreedyConReintentos(
    const std::vector<Procesador>& procesadores,
    const std::vector<Tarea>& tareas,
    const std::vector<Precedencia>& precedencias,
    const std::vector<Comunicacion>& comunicaciones, 
    double alfa, double beta, double gamma, 
    int max_intentos = 10, bool debug = false
);

// Declaración de la función interna (necesaria para que sea visible)
Resultado heuristicaGreedyInterna(
    const std::vector<Procesador>& procesadores,
    const std::vector<Tarea>& tareas,
    const std::vector<Precedencia>& precedencias,
    const std::vector<Comunicacion>& comunicaciones, 
    double alfa, double beta, double gamma, 
    bool usar_seleccion_aleatoria, bool debug = false
);

#endif