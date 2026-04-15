#include "lector.h"
#include <fstream>
#include <iostream>

bool leerDatos(const std::string& filename,
               std::vector<Procesador>& procesadores,
               std::vector<Tarea>& tareas,
               std::vector<Precedencia>& precedencias,
               std::vector<Comunicacion>& comunicaciones) {
    
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: No se pudo abrir el archivo " << filename << std::endl;
        return false;
    }
    
    int n;
    
    // 1. PROCESADORES
    file >> n;
    procesadores.resize(n);
    for (int i = 0; i < n; i++) {
        file >> procesadores[i].id 
             >> procesadores[i].memoria 
             >> procesadores[i].uso
             >> procesadores[i].costo;
    }
    
    // 2. TAREAS
    file >> n;  // n = Number_Tasks-1 
    
    int numero_real_tareas = n + 1;
    
    tareas.resize(numero_real_tareas);
    
    for (int i = 0; i < numero_real_tareas; i++) {
        file >> tareas[i].id
            >> tareas[i].C
            >> tareas[i].T
            >> tareas[i].D
            >> tareas[i].r
            >> tareas[i].M
            >> tareas[i].ProcAss;
        
        if (tareas[i].id != i) {
            std::cerr << "Advertencia: ID de tarea no coincide con índice. "
                      << "Índice: " << i << ", ID: " << tareas[i].id << std::endl;
        }
    }
    
    // 3. PRECEDENCIAS
    file >> n;
    precedencias.resize(n);
    for (int i = 0; i < n; i++) {
        file >> precedencias[i].predecesor 
             >> precedencias[i].sucesor 
             >> precedencias[i].relacion;
    }
    
    // 4. COMUNICACIONES
    file >> n;
    comunicaciones.resize(n);
    for (int i = 0; i < n; i++) {
        file >> comunicaciones[i].source 
             >> comunicaciones[i].destination 
             >> comunicaciones[i].delay;
    }
    
    file.close();
    return true;
}