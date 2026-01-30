#ifndef DATOS_H
#define DATOS_H

#include <string>

// ================= PROCESADOR =================
struct Procesador {
    int id;           // ID del procesador
    double uso;       // Utilización máxima 
    int memoria;      // Memoria disponible en MB
    int costo;        // Costo de uso del procesador 
};

// ================= TAREA =================
struct Tarea {
    int id;          // ID de la tarea
    int C;           // Tiempo de cómputo WCET 
    int T;           // Período 
    int D;           // Deadline absoluto o relativo 
    int r;           // Release time 
    int M;           // Memoria requerida en MB 
    int ProcAss;     // Procesador asignado (0 = sin asignar, >0 = ID procesador MIST)
};

// ================= PRECEDENCIA =================
struct Precedencia {
    int predecesor;  // ID tarea predecesora 
    int sucesor;     // ID tarea sucesora
    int relacion;    // Tipo de relación (0 = normal, 1 = fuerte)
};

// ================= COMUNICACION =================
struct Comunicacion {
    int source;      // ID procesador origen
    int destination; // ID procesador destino
    int delay;       // Retardo de comunicación en unidades de tiempo 
};

#endif