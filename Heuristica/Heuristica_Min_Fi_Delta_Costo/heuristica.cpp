#include "heuristica.h"
#include <algorithm>
#include <limits>
#include <iostream>
#include <iomanip>
#include <map>
#include <queue>
#include <unordered_set>
#include <vector>
#include <cmath>
#include <tuple>

using namespace std;

vector<vector<int>> GenerarMatrizDelta(
    const vector<Comunicacion>& comunicaciones, size_t P
);

pair<map<int, vector<int>>, map<int, vector<int>>> procesarPrecedenciasFuertes(
    const vector<Precedencia>& precedencias
);

void MostrarMatriz(
    const vector<vector<int>>& matriz, size_t P, const bool debug
);

void MostrarPredecesoresSucesores(
    const map<int, 
    vector<int>>& mapa, 
    bool esSucesor, 
    const bool debug
);

void MostrarEstadoProcesadores(
    const vector<Procesador>& procesadores, 
    const vector<int>& memLibre, 
    const vector<double>& uLibre, 
    const vector<int>& costo, 
    const vector<int>& tiempoDisponible, 
    const string& titulo, 
    const bool debug
);

Resultado AsignacionTareasMist(
    const vector<Tarea>& tareas, 
    const map<int, vector<int>>& sucesorAPredecesores, 
    size_t P,
    vector<int>& memLibre, 
    vector<double>& uLibre, 
    vector<bool>& asignado, 
    vector<int>& servidor,
    vector<int>& tiempoDisponible, 
    vector<int>& s, 
    vector<int>& f,
    vector<int>& cost,
    const vector<int>& costosProcesador, 
    const bool debug
);

vector<int> CalcularPredPendientes(
    size_t T, 
    const map<int, vector<int>>& sucesorAPredecesores, 
    const vector<bool>& asignado, 
    const map<int, vector<int>>& predecesorASucesores
);

vector<int> InicializarReadyQueue(
    size_t T, 
    const vector<bool>& asignado, 
    const vector<int>& predPendientes
);

void MostrarReadyQueue(
    const vector<int>& ready, 
    const vector<Tarea>& tareas
);


tuple<int, int, int, int, int> SeleccionarMejorProcesador(
    int tarea_id, 
    const vector<Tarea>& tareas,
    const vector<Procesador>& procesadores,
    const vector<int>& procesadoresAConsiderar,
    const vector<int>& memLibre,
    const vector<double>& uLibre,
    const vector<int>& tiempoDisponible,
    const vector<int>& costosProcesador,
    const map<int, vector<int>>& sucesorAPredecesores,
    const vector<bool>& asignado,
    const vector<int>& servidor,
    const vector<int>& f,
    const vector<vector<int>>& Delta,
    const unordered_set<int>& procesadoresMist,
    const double alfa, 
    const double beta,
    const double gamma, 
    const bool debug
);

void ActualizarRecursos(
    int tarea_id, 
    int procesador_id, 
    const Tarea& tarea, 
    vector<int>& tiempoDisponible,
    vector<int>& memLibre, 
    vector<double>& uLibre, 
    int inicio, 
    int fin, 
    const bool debug
);

void ActualizarReadyQueue(
    int tarea_id, 
    const map<int, vector<int>>& predecesorASucesores, 
    vector<int>& predPendientes,
    vector<bool>& asignado, 
    vector<int>& ready, 
    const vector<Tarea>& tareas, 
    const bool debug
);

bool VerificarDeadlines(
    size_t T, 
    const vector<Tarea>& tareas, 
    const vector<int>& f, 
    const bool debug
);

void MostrarAsignacionFinal(
    size_t T, 
    const vector<Tarea>& tareas, 
    const vector<int>& servidor, 
    const vector<int>& s, 
    const vector<int>& f,
    const vector<int>& cost, 
    const bool debug
);


vector<vector<int>> GenerarMatrizDelta(const vector<Comunicacion>& comunicaciones, size_t P) {
    vector<vector<int>> Delta(P + 1, vector<int>(P + 1, 0));
    for (const auto& c : comunicaciones) {
        if (c.source <= P && c.destination <= P) {
            Delta[c.source][c.destination] = c.delay;
            Delta[c.destination][c.source] = c.delay;
        } else {
            cerr << "Advertencia: ID de procesador fuera de rango en comunicación: "
                 << c.source << "->" << c.destination << endl;
        }
    }
    return Delta;
}

pair<map<int, vector<int>>, map<int, vector<int>>> procesarPrecedenciasFuertes(const vector<Precedencia>& precedencias) {
    map<int, vector<int>> sucesorAPredecesores;
    map<int, vector<int>> predecesorASucesores;
    
    for (const auto& p : precedencias) {
        if (p.relacion == 1) {
            if (p.predecesor >= 0 && p.sucesor >= 0) {
                sucesorAPredecesores[p.sucesor].push_back(p.predecesor);
                predecesorASucesores[p.predecesor].push_back(p.sucesor);
            }
        }
    }
    
    return make_pair(sucesorAPredecesores, predecesorASucesores);
}

void MostrarMatriz(const vector<vector<int>>& matriz, size_t P, const bool debug) {
    if (!debug) return;
    cout << "\033[95m=========================================\033[0m"<< endl;
    cout << "\033[95m             MATRIZ DE DELAYS            \033[0m"<< endl;
    cout << "\033[95m=========================================\033[0m"<< endl;
    cout << "     "; 
    for (size_t j = 1; j <= P; j++) {
        cout << "\033[94m" << setw(4) << j << "\033[0m";
    }
    cout << endl;

    for (size_t i = 1; i <= P; i++) {
        cout << "\033[94m" << setw(3) << i << "\033[0m  ";
        for (size_t j = 1; j <= P; j++) {
            if (matriz[i][j] > 0) {
                cout << "\033[92m" << setw(4) << matriz[i][j] << "\033[0m";
            } else {
                cout << "\033[90m" << setw(4) << matriz[i][j] << "\033[0m";
            }
        }
        cout << endl;
    }
}

void MostrarProcesadoresMist(unordered_set<int> procesadoresMist, const bool debug)
{
    if (!debug) return;
    cout << "\033[95m=========================================\033[0m"<< endl;
    cout << "\033[95m        PROCESADORES MIST IDENTIFICADOS  \033[0m"<< endl;
    cout << "\033[95m=========================================\033[0m"<< endl;

    for (int p : procesadoresMist) cout << "P" << p << " ";
            cout << "\033[0m" << endl;
}

void MostrarPredecesoresSucesores(const map<int, vector<int>>& mapa, bool esSucesor, const bool debug) {
    if (!debug) return;
    string titulo = esSucesor ? "SUCESORES" : "PREDECESORES";
    cout << "\033[95m=========================================\033[0m"<< endl;
    cout << "\033[95m        " << titulo << " DE CADA TAREA       \033[0m" << endl;
    cout << "\033[95m=========================================\033[0m"<< endl;

    for (const auto& par : mapa) {
        cout << "T" << par.first << " " << (esSucesor ? "sucede" : "precede") << " a: [";
        for (size_t i = 0; i < par.second.size(); i++) {
            cout << "T" << par.second[i];
            if (i < par.second.size() - 1) cout << ", ";
        }
        cout << "]" << endl;
    }
}

void MostrarEstadoProcesadores(const vector<Procesador>& procesadores, 
                               const vector<int>& memLibre,  
                               const vector<double>& uLibre, 
                               const vector<int>& costo, 
                               const vector<int>& tiempoDisponible, 
                               const string& titulo, 
                               const bool debug) {
    if (!debug) return;
    cout << "\033[95m=========================================\033[0m" << endl;
    cout << "\033[95m     " << titulo << "     \033[0m" << endl;
    cout << "\033[95m=========================================\033[0m" << endl;
    for (const auto& p : procesadores) {
        cout << "P" << p.id << ": Mem.Libre = " << memLibre[p.id] 
             << "\t Uso = " << fixed << setprecision(3) << uLibre[p.id] 
             << "\t Costo = " << costo[p.id] 
             << "\t Tiempo Inicial = " << tiempoDisponible[p.id] << endl;
    }
}

Resultado AsignacionTareasMist(const vector<Tarea>& tareas, 
                               const map<int, vector<int>>& sucesorAPredecesores, 
                               size_t P,
                               vector<int>& memLibre, 
                               vector<double>& uLibre, 
                               vector<bool>& asignado, 
                               vector<int>& servidor,
                               vector<int>& tiempoDisponible, 
                               vector<int>& s, 
                               vector<int>& f,
                               vector<int>& cost,
                               const vector<int>& costosProcesador, 
                               const bool debug) {
    
    Resultado resultado;
    resultado.factible = true;
    resultado.total_delays = 0.0;
    
    if (debug) {
        cout << "\033[95m=========================================\033[0m" << endl;
        cout << "\033[95m        ASIGNANDO TAREAS MIST          \033[0m" << endl;
        cout << "\033[95m=========================================\033[0m" << endl;
    }
    
    for (const auto& t : tareas) {
        if (t.ProcAss > 0) {
            int tarea_id = t.id;
            
            bool tienePredecesores = (sucesorAPredecesores.find(tarea_id) != sucesorAPredecesores.end() && 
                                    !sucesorAPredecesores.at(tarea_id).empty());
            
            if (!tienePredecesores) {
                int srv = t.ProcAss;
                
                double ui = static_cast<double>(t.C) / t.T;
                
                asignado[tarea_id] = true;
                servidor[tarea_id] = srv;
                s[tarea_id] = tiempoDisponible[srv];
                f[tarea_id] = s[tarea_id] + t.C;
                cost[tarea_id] = costosProcesador[srv];

                // Actualizar recursos del procesador MIST
                tiempoDisponible[srv] = f[tarea_id];
                memLibre[srv] -= t.M;
                uLibre[srv] -= ui;
                if (debug) {
                cout << "\033[92m  [ASIGNADO] T" << tarea_id << " -> P" << srv 
                     << " | Inicio: " << s[tarea_id] << " | Fin: " << f[tarea_id] 
                     << " | Mem usada: " << t.M << " | Uso: " << fixed << setprecision(3) << ui
                     << " | Costo/ud: " << cost[tarea_id] << "\033[0m" << endl;
                }
            } else {
                if (debug) {
                cout << "\033[93m  [POSPUESTO] T" << tarea_id << " tiene " 
                     << sucesorAPredecesores.at(tarea_id).size() << " predecesor(es)\033[0m" << endl;
                }
            }
        }
    }
    
    resultado.servidor = servidor;
    resultado.s = s;
    resultado.f = f;
    resultado.cost = cost;
    
    return resultado;
}

vector<int> CalcularPredPendientes(size_t T, 
                                   const map<int, vector<int>>& sucesorAPredecesores, 
                                   const vector<bool>& asignado, 
                                   const map<int, vector<int>>& predecesorASucesores) {
    vector<int> predPendientes(T, 0);
    
    // Inicializar con número de predecesores
    for (size_t i = 0; i < T; i++) {
        if (sucesorAPredecesores.find(static_cast<int>(i)) != sucesorAPredecesores.end()) {
            predPendientes[i] = sucesorAPredecesores.at(static_cast<int>(i)).size();
        }
    }
    
    // Reducir contador para tareas ya asignadas
    for (size_t i = 0; i < T; i++) {
        if (asignado[i] && predecesorASucesores.find(static_cast<int>(i)) != predecesorASucesores.end()) {
            for (int sucesor : predecesorASucesores.at(static_cast<int>(i))) {
                if (sucesor >= 0 && static_cast<size_t>(sucesor) < T) {
                    predPendientes[sucesor]--;
                }
            }
        }
    }
    
    return predPendientes;
}

vector<int> InicializarReadyQueue(size_t T, 
                                  const vector<bool>& asignado, 
                                  const vector<int>& predPendientes) {
    vector<int> ready;
    for (size_t i = 0; i < T; i++) {
        if (!asignado[i] && predPendientes[i] == 0) {
            ready.push_back(static_cast<int>(i));
        }
    }
    return ready;
}

void MostrarReadyQueue(const vector<int>& ready, 
                       const vector<Tarea>& tareas) {
    cout << "Tareas en ready queue: " << ready.size() << endl;
    for (int t : ready) {
        cout << "* T" << t << " (D=" << tareas[t].D << ", C=" << tareas[t].C 
             << ", M=" << tareas[t].M << ")";
        if (tareas[t].ProcAss > 0) cout << " [MIST al P" << tareas[t].ProcAss << "]";
        cout << endl;
    }
}

tuple<int, int, int, int, int> SeleccionarMejorProcesador(
    int tarea_id, 
    const vector<Tarea>& tareas,
    const vector<Procesador>& procesadores,
    const vector<int>& procesadoresAConsiderar,
    const vector<int>& memLibre,
    const vector<double>& uLibre,
    const vector<int>& tiempoDisponible,
    const vector<int>& costosProcesador,
    const map<int, vector<int>>& sucesorAPredecesores,
    const vector<bool>& asignado,
    const vector<int>& servidor,
    const vector<int>& f,
    const vector<vector<int>>& Delta,
    const unordered_set<int>& procesadoresMist,
    const double alfa, 
    const double beta,
    const double gamma, 
    const bool debug
) {
    
    const int INF = numeric_limits<int>::max();
    int mejorSrv = -1;
    int mejorFin = 0;
    int mejorInicio = 0;
    int mejorCosto = 0;
    int mejorSumDelays = 0;
    
    double mejorValorObjetivo = numeric_limits<double>::max();
    
    if (debug)
        {
            cout << "[PROCESANDO] T" << tarea_id 
                << " (D = " << tareas[tarea_id].D 
                << ", C = " << tareas[tarea_id].C 
                << ", M = " << tareas[tarea_id].M 
                << ", U = " << fixed << setprecision(3) << (static_cast<double>(tareas[tarea_id].C) / tareas[tarea_id].T)
                << ")" << endl;
        }
    
    for (int srv : procesadoresAConsiderar) {

        bool esTareaMist = (tareas[tarea_id].ProcAss > 0);
        
        // Validar restricciones MIST
        if (esTareaMist) {
            if (srv != tareas[tarea_id].ProcAss) {
                if(debug)
                    {
                        cout << "* P" << srv << ": DESCARTADO (tarea MIST solo va a P" 
                     << tareas[tarea_id].ProcAss << ")" << endl;
                    }
                continue;
            }
        } else {
            if (procesadoresMist.find(srv) != procesadoresMist.end()) {
                if (debug)
                    {   
                        cout << "* P" << srv << ": DESCARTADO (procesador MIST no acepta tareas no MIST)" << endl;
                    }
                continue;
            }
        }
        
        int costoActual = costosProcesador[srv];
        double ui = static_cast<double>(tareas[tarea_id].C) / tareas[tarea_id].T;
        int memoriaRequerida = tareas[tarea_id].M;
        
        if (debug)
            {   
                cout << "* P" << srv << ": Mem = " << memLibre[srv] 
                    << ", Uso = " << fixed << setprecision(3) << uLibre[srv]
                    << ", Tiempo Inicio = " << tiempoDisponible[srv]
                    << ", Costo = " << costoActual;
            }
        
        // 1. VERIFICAR RESTRICCIONES DE RECURSOS
        if (memLibre[srv] < memoriaRequerida) {
            if (debug)
                {
                    cout << " -> DESCARTADO (Memoria insuficiente)" << endl;
                }
            continue;
        }
        
        double usoFuturo = uLibre[srv] - ui;
        if (usoFuturo < -1e-9) {
            if (debug)
                {
                    cout << " -> DESCARTADO (Uso excedido)" << endl;
                }
            continue;
        }
        
        // 2. CALCULAR TIEMPO CONSIDERANDO DELAYS
        int maxPredFinConDelay = 0;
        int sumDelays = 0;
        bool predecesoresNoAsignados = false;

        if (sucesorAPredecesores.find(tarea_id) != sucesorAPredecesores.end()) {
            for (int predecesor : sucesorAPredecesores.at(tarea_id)) {
                if (!asignado[predecesor]) {
                    predecesoresNoAsignados = true;
                    break;
                }
                
                int procPredecesor = servidor[predecesor];
                if (procPredecesor >= 0 && srv >= 0 && 
                    procPredecesor < static_cast<int>(Delta.size()) && 
                    srv < static_cast<int>(Delta[procPredecesor].size())) {
                    
                    int delay = Delta[procPredecesor][srv];
                    int finPredecesorConDelay = f[predecesor] + delay;
                    
                    maxPredFinConDelay = max(maxPredFinConDelay, finPredecesorConDelay);
                    sumDelays += delay;
                }
            }
        }
        
        if (predecesoresNoAsignados) {
            if (debug)
                {
                    cout << " -> DESCARTADO (Predecesores no asignados)" << endl;
                }
            continue;
        }
        
        // 3. CALCULAR INICIO Y FIN DE EJECUCIÓN
        int inicio = max(tiempoDisponible[srv], maxPredFinConDelay);
        int fin = inicio + tareas[tarea_id].C;
        
        // 4. VERIFICAR DEADLINE ABSOLUTO
        int deadlineAbsoluto = tareas[tarea_id].r + tareas[tarea_id].D;
        if (fin > deadlineAbsoluto) {
            if (debug)
                {
                    cout << " -> DESCARTADO (Deadline violado: fin=" << fin 
                        << " > deadline=" << deadlineAbsoluto << ")" << endl;
                }
            continue;
        }
        
        // 5. CALCULAR FUNCIÓN OBJETIVO
        double valorObjetivo = alfa * fin + beta * sumDelays +  gamma * costoActual;
        if (debug)
                {
                    cout << " -> Obj = " << fixed << setprecision(2) << valorObjetivo 
                         << " (inicio=" << inicio << ", fin=" << fin << ", suma de delays=" << sumDelays 
                         << ", costo=" << costoActual << ")" << endl;
                }

        // 6. COMPARAR CON MEJOR ACTUAL
        if (valorObjetivo < mejorValorObjetivo - 1e-9) {
            mejorValorObjetivo = valorObjetivo;
            mejorFin = fin;
            mejorInicio = inicio;
            mejorSrv = srv;
            mejorCosto = costoActual;
            mejorSumDelays = sumDelays;
            if (debug)
                {
                    cout << "   -> NUEVO MEJOR (valor=" << mejorValorObjetivo << ")" << endl;
                }
        }
    }
    
    // 7. RETORNAR RESULTADO
    if (mejorSrv != -1 and debug) {
        cout << "[SELECCIONADO] P" << mejorSrv 
             << " con valor objetivo = " << fixed << setprecision(2) << mejorValorObjetivo 
             << " (inicio=" << mejorInicio << ", fin=" << mejorFin 
             << ", costo=" << mejorCosto << ", delays=" << mejorSumDelays << ")" << endl;
    } else {
        if (debug)
        {
            cout << "[ERROR] Ningún procesador válido para T" << tarea_id << endl;
        }
        
    }
    
    return make_tuple(mejorSrv, mejorInicio, mejorFin, mejorSumDelays, mejorCosto);
}

void ActualizarRecursos(int tarea_id, 
                        int procesador_id, 
                        const Tarea& tarea, 
                        vector<int>& tiempoDisponible,
                        vector<int>& memLibre, 
                        vector<double>& uLibre, 
                        int inicio, 
                        int fin, 
                        const bool debug) {

    tiempoDisponible[procesador_id] = fin;
    memLibre[procesador_id] -= tarea.M;
    uLibre[procesador_id] -= static_cast<double>(tarea.C) / tarea.T;
    
    if (debug)
    {
        cout << "-> P" << procesador_id << " ahora: Tiempo Inicial = " << fin 
             << ", Mem Libre = " << memLibre[procesador_id] 
             << ", Uso = " << fixed << setprecision(3) << uLibre[procesador_id] << endl;
    }
    
}

void ActualizarReadyQueue(int tarea_id, 
                          const map<int, vector<int>>& predecesorASucesores, 
                          vector<int>& predPendientes,
                          vector<bool>& asignado, 
                          vector<int>& ready, 
                          const vector<Tarea>& tareas, 
                          const bool debug) {
    
    if (predecesorASucesores.find(tarea_id) != predecesorASucesores.end()) {
        for (int sucesor : predecesorASucesores.at(tarea_id)) {
            if (sucesor >= 0 && static_cast<size_t>(sucesor) < predPendientes.size()) {
                predPendientes[sucesor]--;
                
                if (predPendientes[sucesor] == 0 && !asignado[sucesor]) {
                    ready.push_back(sucesor);
                    if (debug)
                        {
                            cout << "\033[93m[READY+] T" << sucesor << " agregada a ready queue (activada por T" 
                                 << tarea_id << ")\033[0m" << endl;
                        }
                }
            }
        }
    }
}

bool VerificarDeadlines(size_t T, 
                        const vector<Tarea>& tareas, 
                        const vector<int>& f, 
                        const bool debug) {
    for (size_t i = 0; i < T; i++) {
        int deadlineAbsoluto = tareas[i].r + tareas[i].D;
        
        if (f[i] > deadlineAbsoluto) {
            if(debug){
                    cout << "\033[91m[T" << i << "] Violó deadline: fin=" << f[i] 
                        << " > deadline=" << deadlineAbsoluto 
                        << " (r=" << tareas[i].r << ", D=" << tareas[i].D << ")\033[0m" << endl;
                 }
                 
            return false;
        }
    }
    return true;
}

void MostrarAsignacionFinal(size_t T, 
                            const vector<Tarea>& tareas, 
                            const vector<int>& servidor, 
                            const vector<int>& s, 
                            const vector<int>& f,
                            const vector<int>& cost, 
                            const bool debug) {
    
    int costo_total = 0;
    int tiempo_total = 0;
    if (debug)
    {
        cout << "\033[92m=========================================\033[0m" << endl;
        cout << "\033[92m        ASIGNACIÓN COMPLETADA           \033[0m" << endl;
        cout << "\033[92m=========================================\033[0m" << endl;
    }

    
    for (size_t i = 0; i < T; i++) {
        int deadlineAbsoluto = tareas[i].r + tareas[i].D;
        int costo_tarea = cost[i];
        
        costo_total += costo_tarea;
        tiempo_total = max(tiempo_total, f[i]);
        
        bool cumple = (f[i] <= deadlineAbsoluto);
        if (debug)
        {
            cout << "T" << i << " -> P" << servidor[i] 
                << " | Inicio: " << s[i] 
                << " | Fin: " << f[i]
                << " | Costo: " << costo_tarea
                << " | Deadline Abs: " << deadlineAbsoluto 
                << " | " << (cumple ? "\033[92mCUMPLE\033[0m" : "\033[91mNO CUMPLE\033[0m") << endl;
        }
    }
}

// ==========================
// HEURÍSTICA PRINCIPAL
// ==========================
Resultado heuristicaGreedy(
    const vector<Procesador>& procesadores,
    const vector<Tarea>& tareas,
    const vector<Precedencia>& precedencias,
    const vector<Comunicacion>& comunicaciones, 
    const double alfa,
    const double beta,
    const double gamma, 
    const bool debug = false
) {
    size_t T = tareas.size();
    size_t P = procesadores.size();
    
    // Identificar procesadores MIST
    unordered_set<int> procesadoresMist;
    for (const auto& t : tareas) {
        if (t.ProcAss > 0) {
            procesadoresMist.insert(t.ProcAss);
        }
    }

    MostrarProcesadoresMist(procesadoresMist, debug);
    

    // 1. INICIALIZACIÓN
    vector<vector<int>> Delta = GenerarMatrizDelta(comunicaciones, P);
    MostrarMatriz(Delta, P, debug);
    
    auto [sucesorAPredecesores, predecesorASucesores] = procesarPrecedenciasFuertes(precedencias);
    // MostrarPredecesoresSucesores(sucesorAPredecesores, false);
    // MostrarPredecesoresSucesores(predecesorASucesores, true);
    
    // 2. PREPARAR ESTRUCTURAS DE DATOS     
    vector<bool> asignado(T, false);
    vector<int> servidor(T, -1), s(T, -1), f(T, -1);
    vector<int> cost(T, 0);
    vector<double> delays_acumulados(T, 0.0); 
    vector<int> tiempoDisponible(P + 1, 0);  
    vector<int> memLibre(P + 1, 0);
    vector<int> costosProcesador(P + 1, 0);
    vector<double> uLibre(P + 1, 0.0);
    
    for (const auto& p : procesadores) {
        if (p.id >= 0 && static_cast<size_t>(p.id) <= P) {
            memLibre[p.id] = p.memoria;
            uLibre[p.id] = p.uso;
            costosProcesador[p.id] = p.costo;
        }
    }

    MostrarEstadoProcesadores(procesadores, memLibre, uLibre, costosProcesador, tiempoDisponible, "ESTADO INICIAL PROCESADORES", debug);

    // 3. ASIGNAR TAREAS MIST
    Resultado resultadoMist = AsignacionTareasMist(tareas, sucesorAPredecesores, P,
                                                   memLibre, uLibre, asignado, servidor,
                                                   tiempoDisponible, s, f, cost, costosProcesador, debug);
    
    if (!resultadoMist.factible) {
        if (debug) {cout << "\033[91m[ERROR] Falló la asignación de tareas MIST\033[0m" << endl;}
        Resultado resultado_fallo;
        resultado_fallo.factible = false;
        resultado_fallo.total_delays = 0.0;
        return resultado_fallo;
    }
    
    servidor = resultadoMist.servidor;
    s = resultadoMist.s;
    f = resultadoMist.f;
    cost = resultadoMist.cost;
    
    MostrarEstadoProcesadores(procesadores, memLibre, uLibre, costosProcesador, tiempoDisponible, "ESTADO DESPUÉS DE ASIGNAR MIST", debug);
    
    // 4. PREPARAR PARA BUCLE GREEDY
    vector<int> predPendientes = CalcularPredPendientes(T, sucesorAPredecesores, asignado, predecesorASucesores);
    vector<int> ready = InicializarReadyQueue(T, asignado, predPendientes);
    
    if(debug) {
        cout << "\033[95m=========================================\033[0m" << endl;
        cout << "\033[95m            TAREAS EN LA COLA            \033[0m" << endl;
        cout << "\033[95m=========================================\033[0m" << endl;
    }
    
    if (debug) {MostrarReadyQueue(ready, tareas);}  
    
    // 5. BUCLE GREEDY PRINCIPAL
    while (!ready.empty()) {
        if(debug) {
            cout << "\033[95m=========================================\033[0m" << endl;
            cout << "\033[95m     SELECCION DEL MEJOR PROCESADOR      \033[0m" << endl;
            cout << "\033[95m=========================================\033[0m" << endl;
        }

        // Ordenar por deadline más corto
        sort(ready.begin(), ready.end(),
             [&](int a, int b) { return tareas[a].D < tareas[b].D; });
        
        int tarea_actual = ready.front();
        ready.erase(ready.begin());
        
        // Determinar procesadores a considerar
        vector<int> procesadoresAConsiderar;
        
        if (tareas[tarea_actual].ProcAss > 0) {
            procesadoresAConsiderar.push_back(tareas[tarea_actual].ProcAss);
            if(debug) {
                cout << "  [MIST] Solo puede ir al P" << tareas[tarea_actual].ProcAss << endl;
            }
        } else {
            for (const auto& p : procesadores) {
                if (procesadoresMist.find(p.id) == procesadoresMist.end()) {
                    procesadoresAConsiderar.push_back(p.id);
                }
            }
        }
        
        // Seleccionar mejor procesador
        auto [mejorProcesador, inicio_tarea, fin_tarea, delays_tarea_actual, costo_tarea] = SeleccionarMejorProcesador(
            tarea_actual, 
            tareas,
            procesadores,
            procesadoresAConsiderar,
            memLibre, 
            uLibre, 
            tiempoDisponible,
            costosProcesador,
            sucesorAPredecesores, 
            asignado,
            servidor, 
            f, 
            Delta,
            procesadoresMist, 
            alfa, 
            beta, 
            gamma, 
            debug
        );
        
        if (mejorProcesador == -1) {
            if (debug) {cout << "\033[91m[ERROR] T" << tarea_actual << " no asignable a ningún procesador\033[0m" << endl;}
            Resultado resultado_fallo;
            resultado_fallo.factible = false;
            resultado_fallo.total_delays = 0.0;
            return resultado_fallo;
        }
        
        // Asignar la tarea
        asignado[tarea_actual] = true;
        servidor[tarea_actual] = mejorProcesador;
        s[tarea_actual] = inicio_tarea;
        f[tarea_actual] = fin_tarea;  // Usar el fin calculado
        cost[tarea_actual] = costo_tarea;
        delays_acumulados[tarea_actual] = delays_tarea_actual;
        
        // Actualizar recursos
        ActualizarRecursos(tarea_actual, mejorProcesador, tareas[tarea_actual], 
                        tiempoDisponible, memLibre, uLibre, inicio_tarea, fin_tarea, debug);

        if(debug){
            cout << "\033[92m[ASIGNADO] T" << tarea_actual << " -> P" << mejorProcesador 
                << " | Inicio: " << inicio_tarea << " | Fin: " << fin_tarea 
                << " | Costo/ud: " << costo_tarea 
                << " | Delays acumulados: " << delays_tarea_actual 
                << " | Costo total: " << (costo_tarea) << "\033[0m" << endl;
            }
        
        // Actualizar ready queue
        ActualizarReadyQueue(tarea_actual, predecesorASucesores, predPendientes, asignado, ready, tareas, debug);
        
        if (debug) {
            cout << "\033[95m=========================================\033[0m" << endl;
            cout << "\033[95m            TAREAS EN LA COLA            \033[0m" << endl;
            cout << "\033[95m=========================================\033[0m" << endl;

            MostrarReadyQueue(ready, tareas);
        }

        
    }
    
    // 6. VERIFICAR ASIGNACIÓN COMPLETA
    for (size_t i = 0; i < T; i++) {
        if (!asignado[i]) {
            if (debug) {cout << "\033[91m[ERROR] T" << i << " no fue asignada\033[0m" << endl;}
            Resultado resultado_fallo;
            resultado_fallo.factible = false;
            resultado_fallo.total_delays = 0.0;
            return resultado_fallo;
        }
    }
    
    // 7. CALCULAR SUMA TOTAL DE DELAYS
    double total_delays = 0.0;
    for (size_t i = 0; i < T; i++) {
        total_delays += delays_acumulados[i];
    }
    
    // 8. VERIFICAR DEADLINES Y MOSTRAR RESULTADO
    bool todasCumplen = VerificarDeadlines(T, tareas, f, debug);
    MostrarAsignacionFinal(T, tareas, servidor, s, f, cost, debug);
    
    // 9. CREAR Y RETORNAR RESULTADO COMPLETO
    Resultado resultado_final;
    resultado_final.servidor = servidor;
    resultado_final.s = s;
    resultado_final.f = f;
    resultado_final.cost = cost;
    resultado_final.total_delays = total_delays;
    resultado_final.factible = todasCumplen;
    
    return resultado_final;
}
