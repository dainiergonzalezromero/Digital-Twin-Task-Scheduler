#include "lector.h"
#include <fstream>

bool leerDatos(const std::string& filename,
               std::vector<Procesador>& procesadores,
               std::vector<Tarea>& tareas,
               std::vector<Precedencia>& precedencias,
               std::vector<Comunicacion>& comunicaciones) {
    
    std::ifstream file(filename);
    if (!file) return false;
    
    int n;
    
    // Procesadores
    file >> n;
    procesadores.resize(n);
    for (int i = 0; i < n; i++) {
        file >> procesadores[i].id 
             >> procesadores[i].memoria 
             >> procesadores[i].U;
    }
    
    // Tareas
    file >> n;     // n = último ID
    n = n + 1;     // ahora n = cantidad real

    tareas.resize(n);

    for (int i = 0; i < n; i++) {
        file >> tareas[i].id
            >> tareas[i].C
            >> tareas[i].T
            >> tareas[i].D
            >> tareas[i].r
            >> tareas[i].M
            >> tareas[i].ProcAss;
    }

    
    // Precedencias
    file >> n;
    precedencias.resize(n);
    for (int i = 0; i < n; i++) {
        file >> precedencias[i].predecesor 
             >> precedencias[i].sucesor 
             >> precedencias[i].relacion;
    }
    
    // Comunicaciones
    file >> n;
    comunicaciones.resize(n);
    for (int i = 0; i < n; i++) {
        file >> comunicaciones[i].source 
             >> comunicaciones[i].destination 
             >> comunicaciones[i].delay;
    }
    
    return true;
}