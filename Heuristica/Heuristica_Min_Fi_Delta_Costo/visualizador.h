#ifndef VISUALIZADOR_H
#define VISUALIZADOR_H

#include <vector>
#include <string>
#include "datos.h"

// ============================================
// 1. RESUMEN GENERAL
// ============================================
void mostrarResumenGeneral(
    const std::vector<Procesador>& procesadores,
    const std::vector<Tarea>& tareas,
    const std::vector<Precedencia>& precedencias,
    const std::vector<Comunicacion>& comunicaciones,
    const std::string& nombreArchivo
);

// ============================================
// 2. PROCESADORES
// ============================================
void mostrarProcesadoresDetallados(
    const std::vector<Procesador>& procesadores
);

// ============================================
// 3. TAREAS
// ============================================
void mostrarTareasDetalladas(
    const std::vector<Tarea>& tareas,
    int limite
);

// ============================================
// 4. PRECEDENCIAS
// ============================================
void mostrarPrecedenciasDetalladas(
    const std::vector<Precedencia>& precedencias,
    int limite
);

// ============================================
// 5. COMUNICACIONES
// ============================================
void mostrarComunicacionesDetalladas(
    const std::vector<Comunicacion>& comunicaciones,
    int limite
);



#endif // VISUALIZADOR_H
