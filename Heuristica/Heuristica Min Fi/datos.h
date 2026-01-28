#ifndef DATOS_H
#define DATOS_H

#include <string>

// ================= PROCESADOR =================
struct Procesador {
    int id;
    double U;          // Utilización
    int memoria;       // Memoria disponible
};

// ================= TAREA =================
struct Tarea {
    int id;
    int C;   // Tiempo de cómputo
    int T;   // Periodo
    int D;   // Deadline
    int r;   // Release time
    int M;   // Memoria requerida
    int ProcAss;   // Procesador asignado (0 = sin asignar)
};

// ================= PRECEDENCIA =================
struct Precedencia {
    int predecesor;
    int sucesor;
    int relacion;      // 0 normal, 1 fuerte
};

// ================= COMUNICACION =================
struct Comunicacion {
    int source;
    int destination;
    int delay;
};

#endif
