#include "visualizador.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <climits>

using namespace std;

// ============================================
// 1. FUNCIÓN PRINCIPAL - RESUMEN GENERAL
// ============================================
void mostrarResumenGeneral(
    const vector<Procesador>& procesadores,
    const vector<Tarea>& tareas,
    const vector<Precedencia>& precedencias,
    const vector<Comunicacion>& comunicaciones,
    const string& nombreArchivo
) {
    cout << "\n" << string(80, '=') << endl;
    cout << " RESUMEN GENERAL - " << nombreArchivo << endl;
    cout << string(80, '=') << endl;

    cout << "\n+--------------------------------------------------+" << endl;
    cout << "|              RESUMEN NUMERICO                    |" << endl;
    cout << "+--------------------------------------------------+" << endl;
    cout << "|  Procesadores: " << setw(4) << procesadores.size() << "                              |" << endl;
    cout << "|  Tareas:       " << setw(4) << tareas.size()
         << " (IDs: 0 a " << setw(3) << tareas.size()-1 << ")               |" << endl;
    cout << "|  Precedencias: " << setw(4) << precedencias.size() << "                              |" << endl;
    cout << "|  Comunicaciones:" << setw(3) << comunicaciones.size() << "                              |" << endl;
    cout << "+--------------------------------------------------+" << endl;

    map<string,int> tiposProcesadores;
    for (const auto& p : procesadores) {
        if (p.memoria <= 15) tiposProcesadores["Mist"]++;
        else if (p.memoria < 100) tiposProcesadores["Edge"]++;
        else if (p.memoria < 1000) tiposProcesadores["Fog"]++;
        else tiposProcesadores["Cloud"]++;
    }

    cout << "\n+--------------------------------------------------+" << endl;
    cout << "|             TIPOS DE PROCESADORES                |" << endl;
    cout << "+--------------------------------------------------+" << endl;
    for (const auto& [tipo,cantidad] : tiposProcesadores) {
        double porcentaje = (cantidad * 100.0) / procesadores.size();
        cout << "|  - " << left << setw(10) << tipo
             << ": " << setw(3) << cantidad
             << " (" << fixed << setprecision(1) << setw(4) << porcentaje << "%)                       |" << endl;
    }
    cout << "+--------------------------------------------------+" << endl;

    int preAsignadas = 0, sinAsignar = 0, totalMemoria = 0;
    for (const auto& t : tareas) {
        if (t.ProcAss > 0) preAsignadas++;
        else sinAsignar++;
        totalMemoria += t.M;
    }

    cout << "\n+--------------------------------------------------+" << endl;
    cout << "|              ESTADO DE LAS TAREAS                |" << endl;
    cout << "+--------------------------------------------------+" << endl;
    cout << "|  Pre-asignadas: " << setw(4) << preAsignadas << " | "
         << fixed << setprecision(1) << (preAsignadas*100.0/tareas.size()) << "%                     |" << endl;
    cout << "|  Sin asignar:   " << setw(4) << sinAsignar << " | "
         << fixed << setprecision(1) << (sinAsignar*100.0/tareas.size()) << "%                     |" << endl;
    cout << "|  Memoria total: " << setw(6) << totalMemoria << " MB                        |" << endl;
    cout << "+--------------------------------------------------+" << endl;
}

// ============================================
// 2. PROCESADORES
// ============================================
void mostrarProcesadoresDetallados(const vector<Procesador>& procesadores) {
    cout << "\n" << string(80, '=') << endl;
    cout << " DETALLE DE PROCESADORES (" << procesadores.size() << ")" << endl;
    cout << string(80, '=') << endl;

    cout << "+-----+----------+----------+----------+--------+---------+" << endl;
    cout << "| ID  | Memoria  |    U     |   Tipo   |Mem Acum| % Uso   |" << endl;
    cout << "+-----+----------+----------+----------+--------+---------+" << endl;

    int totalMem = 0, acum = 0;
    for (const auto& p : procesadores) totalMem += p.memoria;

    for (const auto& p : procesadores) {
        acum += p.memoria;
        double uso = (p.memoria * 100.0) / totalMem;

        string tipo;
        if (p.memoria <= 15) tipo = "Mist";
        else if (p.memoria < 100) tipo = "Edge";
        else if (p.memoria < 1000) tipo = "Fog";
        else tipo = "Cloud";

        cout << "| " << setw(3) << p.id
             << " | " << setw(8) << p.memoria
             << " | " << setw(8) << fixed << setprecision(3) << p.U
             << " | " << setw(8) << tipo
             << " | " << setw(6) << acum 
             << " | " << setw(6) << uso << "% |" << endl;
    }
    cout << "+-----+----------+----------+----------+--------+---------+" << endl;
}

// ============================================
// 3. TAREAS
// ============================================
void mostrarTareasDetalladas(const vector<Tarea>& tareas, int limite) {
    cout << "\n" << string(80, '=') << endl;
    cout << " DETALLE DE TAREAS (" << tareas.size() << ")" << endl;
    cout << string(80, '=') << endl;

    int mostrar = min(limite, (int)tareas.size());

    cout << "+-------+----------+----------+----------+--------------+----------+" << endl;
    cout << "|  ID   | C  |    T     |   D   | r     | M    |" << endl;
    cout << "+-------+----------+----------+----------+--------------+----------+" << endl;

    for (int i = 0; i < mostrar; i++) {
        const auto& t = tareas[i];
        cout << "" << setw(3) << t.id
             << "  " << setw(3) << t.C
             << "  " << setw(3) << t.T
             << "  " << setw(3) << t.D
             << "  " << setw(3) << t.r
             << "  " << setw(4) << t.M
             << "  " << setw(3) << t.ProcAss << endl;
    }
}

// ============================================
// 4. PRECEDENCIAS
// ============================================
void mostrarPrecedenciasDetalladas(
    const vector<Precedencia>& precedencias,
    int limite
) {
    cout << "\nPRECEDENCIAS:" << endl;

    int i = 0;
    int mostradas = 0;
    int n = precedencias.size();

    while (i < n && mostradas < limite) {
        const auto& p = precedencias[i];

        if (p.relacion == 1) {
            cout << "T" << p.predecesor
                 << " -> T" << p.sucesor << endl;
            mostradas++;  
        }

        i++; 
    }
}


// ============================================
// 5. COMUNICACIONES
// ============================================
void mostrarComunicacionesDetalladas(
    const vector<Comunicacion>& comunicaciones,
    int limite
) {
    cout << "\nCOMUNICACIONES:" << endl;

    int i = 0;
    int mostradas = 0;
    int n = comunicaciones.size();

    while (i < n && mostradas < limite) {
        const auto& c = comunicaciones[i];

        if (c.delay < 1000 and c.delay > 0) {
            cout << "P" << c.source
                 << " -> P" << c.destination
                 << " Delay =" << c.delay << endl;
            mostradas++;   // 👈 solo cuenta las válidas
        }

        i++; // 👈 siempre avanzamos
    }
}

