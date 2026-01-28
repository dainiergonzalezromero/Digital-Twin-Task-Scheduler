#include "heuristica.h"
#include <algorithm>
#include <limits>
#include <iostream>
#include <iomanip> 
#include <map>
#include <queue>

using namespace std;

// ==========================
// DECLARACIONES DE FUNCIONES AUXILIARES
// ==========================
vector<vector<int>> GenerarMatrizDelta(const vector<Comunicacion>& comunicaciones, int P);
pair<map<int, vector<int>>, map<int, vector<int>>> procesarPrecedenciasFuertes(const vector<Precedencia>& precedencias);
void MostrarMatriz(const vector<vector<int>>& matriz, int P);
void MostrarPredecesoresSucesores(const map<int, vector<int>>& mapa, bool esSucesor);
void MostrarEstadoProcesadores(const vector<Procesador>& procesadores, const vector<int>& memLibre, const vector<double>& uLibre, const vector<int>& tiempoDisponible, const string& titulo);
Resultado AsignacionTareasMist(const vector<Tarea>& tareas, const map<int, vector<int>>& sucesorAPredecesores, int P,
                               vector<int>& memLibre, vector<double>& uLibre, vector<bool>& asignado, vector<int>& servidor,
                               vector<int>& tiempoDisponible, vector<int>& s, vector<int>& f);
vector<int> CalcularPredPendientes(int T, const map<int, vector<int>>& sucesorAPredecesores, 
                                   const vector<bool>& asignado, const map<int, vector<int>>& predecesorASucesores);
vector<int> InicializarReadyQueue(int T, const vector<bool>& asignado, const vector<int>& predPendientes);
void MostrarReadyQueue(const vector<int>& ready, const vector<Tarea>& tareas);
pair<int, int> SeleccionarMejorProcesador(int tarea_id, const vector<Tarea>& tareas, const vector<int>& procesadoresAConsiderar,
                                          const vector<int>& memLibre, const vector<double>& uLibre, const vector<int>& tiempoDisponible,
                                          const map<int, vector<int>>& sucesorAPredecesores, const vector<bool>& asignado,
                                          const vector<int>& servidor, const vector<int>& f, const vector<vector<int>>& Delta);
void ActualizarRecursos(int tarea_id, int procesador_id, const Tarea& tarea, vector<int>& tiempoDisponible,
                        vector<int>& memLibre, vector<double>& uLibre, int inicio, int fin);
void ActualizarReadyQueue(int tarea_id, const map<int, vector<int>>& predecesorASucesores, vector<int>& predPendientes,
                          vector<bool>& asignado, vector<int>& ready, const vector<Tarea>& tareas);
bool VerificarDeadlines(int T, const vector<Tarea>& tareas, const vector<int>& f);
void MostrarAsignacionFinal(int T, const vector<Tarea>& tareas, const vector<int>& servidor, 
                            const vector<int>& s, const vector<int>& f);

// ==========================
// IMPLEMENTACIONES DE FUNCIONES AUXILIARES
// ==========================

vector<vector<int>> GenerarMatrizDelta(const vector<Comunicacion>& comunicaciones, int P) {
    vector<vector<int>> Delta(P + 1, vector<int>(P + 1, 0));
    for (const auto& c : comunicaciones) {
        Delta[c.source][c.destination] = c.delay;
        Delta[c.destination][c.source] = c.delay;
    }
    return Delta;
}

pair<map<int, vector<int>>, map<int, vector<int>>> procesarPrecedenciasFuertes(const vector<Precedencia>& precedencias) {
    map<int, vector<int>> sucesorAPredecesores;
    map<int, vector<int>> predecesorASucesores;
    
    for (const auto& p : precedencias) {
        if (p.relacion == 1) {
            sucesorAPredecesores[p.sucesor].push_back(p.predecesor);
            predecesorASucesores[p.predecesor].push_back(p.sucesor);
        }
    }
    
    return make_pair(sucesorAPredecesores, predecesorASucesores);
}

void MostrarMatriz(const vector<vector<int>>& matriz, int P) {
    cout << "\033[95m=========================================\033[0m"<< endl;
    cout << "\033[95m             MATRIZ DE DELAYS            \033[0m"<< endl;
    cout << "\033[95m=========================================\033[0m"<< endl;
    cout << "     "; 
    for (int j = 1; j <= P; j++) {
        cout << "\033[94m" << setw(4) << j << "\033[0m";
    }
    cout << endl;

    for (int i = 1; i <= P; i++) {
        cout << "\033[94m" << setw(3) << i << "\033[0m  ";
        for (int j = 1; j <= P; j++) {
            if (matriz[i][j] > 0) {
                cout << "\033[92m" << setw(4) << matriz[i][j] << "\033[0m";
            } else {
                cout << "\033[90m" << setw(4) << matriz[i][j] << "\033[0m";
            }
        }
        cout << endl;
    }
}

void MostrarPredecesoresSucesores(const map<int, vector<int>>& mapa, bool esSucesor) {
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

void MostrarEstadoProcesadores(const vector<Procesador>& procesadores, const vector<int>& memLibre, 
                               const vector<double>& uLibre, const vector<int>& tiempoDisponible, const string& titulo) {
    cout << "\033[95m=========================================\033[0m" << endl;
    cout << "\033[95m     " << titulo << "     \033[0m" << endl;
    cout << "\033[95m=========================================\033[0m" << endl;
    for (const auto& p : procesadores) {
        cout << "P" << p.id << ": Mem.Libre = " << memLibre[p.id] 
             << "\t Uso = " << fixed << setprecision(3) << uLibre[p.id] 
             << "\t Tiempo Inicial = " << tiempoDisponible[p.id] << endl;
    }
}

Resultado AsignacionTareasMist(const vector<Tarea>& tareas, const map<int, vector<int>>& sucesorAPredecesores, int P,
                               vector<int>& memLibre, vector<double>& uLibre, vector<bool>& asignado, vector<int>& servidor,
                               vector<int>& tiempoDisponible, vector<int>& s, vector<int>& f) {
    
    Resultado resultado;
    resultado.factible = true;
    
    cout << "\033[95m=========================================\033[0m" << endl;
    cout << "\033[95m        ASIGNANDO TAREAS MIST          \033[0m" << endl;
    cout << "\033[95m=========================================\033[0m" << endl;
    
    for (const auto& t : tareas) {
        if (t.ProcAss > 0) {
            int tarea_id = t.id;
            
            bool tienePredecesores = (sucesorAPredecesores.find(tarea_id) != sucesorAPredecesores.end() && 
                                    !sucesorAPredecesores.at(tarea_id).empty());
            
            if (!tienePredecesores) {
                int srv = t.ProcAss;
                
                if (srv < 1 || srv > P) {
                    cout << "\033[91m[ERROR] T" << tarea_id << " tiene procesador MIST inválido: " << srv << "\033[0m" << endl;
                    resultado.factible = false;
                    return resultado;
                }
                
                double ui = (double)t.C / t.T;
                
                if (memLibre[srv] < t.M) {
                    cout << "\033[91m[ERROR] T" << tarea_id << " (MIST) no cabe en P" << srv 
                         << " - Memoria: " << memLibre[srv] << " < " << t.M << "\033[0m" << endl;
                    resultado.factible = false;
                    return resultado;
                }
                
                if (uLibre[srv] + ui > 1.0 + 1e-9) {
                    cout << "\033[91m[ERROR] T" << tarea_id << " (MIST) excede uso en P" << srv 
                         << " - Uso: " << uLibre[srv] << " + " << ui << " = " 
                         << (uLibre[srv] + ui) << " > 1.0\033[0m" << endl;
                    resultado.factible = false;
                    return resultado;
                }
                
                // Asignar
                asignado[tarea_id] = true;
                servidor[tarea_id] = srv;
                s[tarea_id] = tiempoDisponible[srv];
                f[tarea_id] = s[tarea_id] + t.C;

                tiempoDisponible[srv] = f[tarea_id];
                memLibre[srv] -= t.M;
                uLibre[srv] += ui;
                
                cout << "\033[92m  [ASIGNADO] T" << tarea_id << " -> P" << srv 
                     << " | Inicio: " << s[tarea_id] << " | Fin: " << f[tarea_id] 
                     << " | Mem usada: " << t.M << "\033[0m" << endl;
            } else {
                cout << "\033[93m  [POSPUESTO] T" << tarea_id << " tiene " 
                     << sucesorAPredecesores.at(tarea_id).size() << " predecesor(es)\033[0m" << endl;
            }
        }
    }
    
    resultado.servidor = servidor;
    resultado.s = s;
    resultado.f = f;
    
    return resultado;
}

vector<int> CalcularPredPendientes(int T, const map<int, vector<int>>& sucesorAPredecesores, 
                                   const vector<bool>& asignado, const map<int, vector<int>>& predecesorASucesores) {
    vector<int> predPendientes(T, 0);
    
    // Inicializar con número de predecesores
    for (int i = 0; i < T; i++) {
        if (sucesorAPredecesores.find(i) != sucesorAPredecesores.end()) {
            predPendientes[i] = sucesorAPredecesores.at(i).size();
        }
    }
    
    // Reducir contador para tareas ya asignadas
    for (int i = 0; i < T; i++) {
        if (asignado[i] && predecesorASucesores.find(i) != predecesorASucesores.end()) {
            for (int sucesor : predecesorASucesores.at(i)) {
                predPendientes[sucesor]--;
            }
        }
    }
    
    return predPendientes;
}

vector<int> InicializarReadyQueue(int T, const vector<bool>& asignado, const vector<int>& predPendientes) {
    vector<int> ready;
    for (int i = 0; i < T; i++) {
        if (!asignado[i] && predPendientes[i] == 0) {
            ready.push_back(i);
        }
    }
    return ready;
}

void MostrarReadyQueue(const vector<int>& ready, const vector<Tarea>& tareas) {
    cout << "Tareas en ready queue: " << ready.size() << endl;
    for (int t : ready) {
        cout << "* T" << t << " (D=" << tareas[t].D << ", C=" << tareas[t].C 
             << ", M=" << tareas[t].M << ")";
        if (tareas[t].ProcAss > 0) cout << " [MIST al P" << tareas[t].ProcAss << "]";
        cout << endl;
    }
}

pair<int, int> SeleccionarMejorProcesador(int tarea_id, const vector<Tarea>& tareas, const vector<int>& procesadoresAConsiderar,
                                          const vector<int>& memLibre, const vector<double>& uLibre, const vector<int>& tiempoDisponible,
                                          const map<int, vector<int>>& sucesorAPredecesores, const vector<bool>& asignado,
                                          const vector<int>& servidor, const vector<int>& f, const vector<vector<int>>& Delta) {
    
    const int INF = numeric_limits<int>::max();
    int mejorSrv = -1;
    int mejorFin = INF;
    double mejorUso = 2.0;
    int mejorMemoriaLibre = -1;
    int mejorInicio = INF;
    
    cout << "[PROCESANDO] T" << tarea_id 
         << " (D = " << tareas[tarea_id].D 
         << ", C = " << tareas[tarea_id].C 
         << ", M = " << tareas[tarea_id].M 
         << ", U = " << fixed << setprecision(3) << ((double)tareas[tarea_id].C / tareas[tarea_id].T)
         << ")" << endl;
    
    if (tareas[tarea_id].ProcAss > 0) {
        cout << "[MIST] Solo puede ir al P" << tareas[tarea_id].ProcAss << endl;
    } else {
        cout << "Evaluando todos los procesadores" << endl;
    }
    
    for (int srv : procesadoresAConsiderar) {
        // VERIFICACIÓN MIST CRUCIAL:
        // Si la tarea NO es MIST (L == 0) pero el procesador es MIST (srv <= maxMist), entonces NO puede usarlo
        // Necesitamos saber cuál es el máximo procesador MIST
        
        // Encontrar el máximo procesador MIST basado en los valores L de las tareas
        int maxMistProcessor = 0;
        for (const auto& t : tareas) {
            if (t.ProcAss > maxMistProcessor) {
                maxMistProcessor = t.ProcAss;
            }
        }
        
        // Si el procesador es MIST (srv <= maxMistProcessor) pero la tarea NO es MIST para este procesador
        if (srv <= maxMistProcessor && tareas[tarea_id].ProcAss != srv) {
            cout << "* P" << srv << ": Mem = " << memLibre[srv] 
                 << " -> DESCARTADO (procesador MIST P" << srv 
                 << " solo acepta tareas MIST para P" << srv << ")" << endl;
            continue;
        }
        
        double ui = (double)tareas[tarea_id].C / tareas[tarea_id].T;
        int memoriaRequerida = tareas[tarea_id].M;
        
        cout << "* P" << srv << ": Mem = " << memLibre[srv] 
             << ", Uso = " << fixed << setprecision(3) << uLibre[srv]
             << ", Tiempo Inicio = " << tiempoDisponible[srv];
        
        // 1. VERIFICAR RESTRICCIONES DE RECURSOS
        if (memLibre[srv] < memoriaRequerida) {
            cout << " -> DESCARTADO (memoria)" << endl;
            continue;
        }
        
        double usoFuturo = uLibre[srv] + ui;
        if (usoFuturo > 1.0 + 1e-9) {
            cout << " -> DESCARTADO (uso: " << fixed << setprecision(3) << usoFuturo << " > 1.0)" << endl;
            continue;
        }
        
        // 2. CALCULAR TIEMPO DE INICIO CONSIDERANDO DELAYS
        int maxPredFinConDelay = 0;
        bool predecesoresNoAsignados = false;
        
        if (sucesorAPredecesores.find(tarea_id) != sucesorAPredecesores.end()) {
            for (int predecesor : sucesorAPredecesores.at(tarea_id)) {
                if (!asignado[predecesor]) {
                    predecesoresNoAsignados = true;
                    break;
                }
                
                int delay = Delta[servidor[predecesor]][srv];
                int finPredecesorConDelay = f[predecesor] + delay;
                maxPredFinConDelay = max(maxPredFinConDelay, finPredecesorConDelay);
            }
        }
        
        if (predecesoresNoAsignados) {
            cout << " -> DESCARTADO (predecesores no asignados)" << endl;
            continue;
        }
        
        // 3. CALCULAR TIEMPO DE INICIO Y FIN
        int inicio = max(tiempoDisponible[srv], maxPredFinConDelay);
        int fin = inicio + tareas[tarea_id].C;
        
        // 4. VERIFICAR DEADLINE
        int deadlineLimite = tareas[tarea_id].D;
        if (tareas[tarea_id].r > 0) {
            deadlineLimite = tareas[tarea_id].r + tareas[tarea_id].D;
        }
        
        if (fin > deadlineLimite) {
            cout << " -> DESCARTADO (deadline: " << fin << " > " << deadlineLimite << ")" << endl;
            continue;
        }
        
        // 5. CRITERIOS DE SELECCIÓN
        bool esMejor = false;
        
        if (fin < mejorFin) {
            esMejor = true;
        } 
        else if (fin == mejorFin) {
            if (usoFuturo < mejorUso - 1e-9) {
                esMejor = true;
            }
            else if (abs(usoFuturo - mejorUso) < 1e-9) {
                int memoriaLibreDespues = memLibre[srv] - memoriaRequerida;
                if (memoriaLibreDespues > mejorMemoriaLibre) {
                    esMejor = true;
                }
            }
        }
        
        if (esMejor) {
            mejorFin = fin;
            mejorInicio = inicio;
            mejorSrv = srv;
            mejorUso = usoFuturo;
            mejorMemoriaLibre = memLibre[srv] - memoriaRequerida;
            
            cout << " -> MEJOR CANDIDATO (Inicio = " << inicio 
                 << ", Fin = " << fin 
                 << ", Uso = " << fixed << setprecision(3) << usoFuturo 
                 << ", Mem Libre = " << mejorMemoriaLibre << ")" << endl;
        } else {
            cout << " -> NO MEJOR" << endl;
        }
    }
    
    return {mejorSrv, mejorInicio};
}

void ActualizarRecursos(int tarea_id, int procesador_id, const Tarea& tarea, vector<int>& tiempoDisponible,
                        vector<int>& memLibre, vector<double>& uLibre, int inicio, int fin) {

    tiempoDisponible[procesador_id] = fin;
    memLibre[procesador_id] -= tarea.M;
    uLibre[procesador_id] += (double)tarea.C / tarea.T;
    
    cout << "-> P" << procesador_id << " ahora: Tiempo Inicial = " << fin 
         << ", Mem Libre = " << memLibre[procesador_id] 
         << ", Uso = " << fixed << setprecision(3) << uLibre[procesador_id] << endl;
}

void ActualizarReadyQueue(int tarea_id, const map<int, vector<int>>& predecesorASucesores, vector<int>& predPendientes,
                          vector<bool>& asignado, vector<int>& ready, const vector<Tarea>& tareas) {
    
    if (predecesorASucesores.find(tarea_id) != predecesorASucesores.end()) {
        for (int sucesor : predecesorASucesores.at(tarea_id)) {
            predPendientes[sucesor]--;
            
            if (predPendientes[sucesor] == 0 && !asignado[sucesor]) {
                ready.push_back(sucesor);
                cout << "\033[93m[READY+] T" << sucesor << " agregada a ready queue (activada por T" 
                     << tarea_id << ")\033[0m" << endl;
            }
        }
    }
}

bool VerificarDeadlines(int T, const vector<Tarea>& tareas, const vector<int>& f) {
    for (int i = 0; i < T; i++) {
        int deadlineLimite = tareas[i].D;
        if (tareas[i].r > 0) {
            deadlineLimite = tareas[i].r + tareas[i].D;
        }
        
        if (f[i] > deadlineLimite) {
            return false;
        }
    }
    return true;
}

void MostrarAsignacionFinal(int T, const vector<Tarea>& tareas, const vector<int>& servidor, 
                            const vector<int>& s, const vector<int>& f) {
    cout << "\033[92m=========================================\033[0m" << endl;
    cout << "\033[92m        ASIGNACIÓN COMPLETADA           \033[0m" << endl;
    cout << "\033[92m=========================================\033[0m" << endl;
    
    for (int i = 0; i < T; i++) {
        int deadlineLimite = tareas[i].D;
        if (tareas[i].r > 0) {
            deadlineLimite = tareas[i].r + tareas[i].D;
        }
        
        bool cumple = (f[i] <= deadlineLimite);
        cout << "T" << i << " -> P" << servidor[i] 
             << " | Inicio: " << s[i] 
             << " | Fin: " << f[i] 
             << " | D: " << deadlineLimite 
             << " | " << (cumple ? "\033[92mCUMPLE\033[0m" : "\033[91mNO CUMPLE\033[0m") << endl;
    }
}

// ==========================
// HEURÍSTICA PRINCIPAL (SOLO UNA DEFINICIÓN)
// ==========================
Resultado heuristicaGreedy(
    const vector<Procesador>& procesadores,
    const vector<Tarea>& tareas,
    const vector<Precedencia>& precedencias,
    const vector<Comunicacion>& comunicaciones
) {
    int T = tareas.size();
    int P = procesadores.size();
    
    // 1. INICIALIZACIÓN
    vector<vector<int>> Delta = GenerarMatrizDelta(comunicaciones, P);
    MostrarMatriz(Delta, P);
    
    auto [sucesorAPredecesores, predecesorASucesores] = procesarPrecedenciasFuertes(precedencias);
    MostrarPredecesoresSucesores(sucesorAPredecesores, false);
    MostrarPredecesoresSucesores(predecesorASucesores, true);
    
    // 2. PREPARAR ESTRUCTURAS DE DATOS
    vector<bool> asignado(T, false);
    vector<int> servidor(T, -1), s(T, -1), f(T, -1);
    vector<int> tiempoDisponible(P + 1, 0);
    vector<int> memLibre(P + 1, 0);
    vector<double> uLibre(P + 1, 0.0);
    
    for (const auto& p : procesadores) {
        memLibre[p.id] = p.memoria;
    }
    
    MostrarEstadoProcesadores(procesadores, memLibre, uLibre, tiempoDisponible, "ESTADO INICIAL PROCESADORES");
    
    // 3. ASIGNAR TAREAS MIST
    Resultado resultadoMist = AsignacionTareasMist(tareas, sucesorAPredecesores, P,
                                                   memLibre, uLibre, asignado, servidor,
                                                   tiempoDisponible, s, f);
    
    if (!resultadoMist.factible) {
        cout << "\033[91m[ERROR] Falló la asignación de tareas MIST\033[0m" << endl;
        return {{}, {}, {}, false};
    }
    
    servidor = resultadoMist.servidor;
    s = resultadoMist.s;
    f = resultadoMist.f;
    
    MostrarEstadoProcesadores(procesadores, memLibre, uLibre, tiempoDisponible, "ESTADO DESPUÉS DE ASIGNAR MIST");
    
    // 4. PREPARAR PARA BUCLE GREEDY
    vector<int> predPendientes = CalcularPredPendientes(T, sucesorAPredecesores, asignado, predecesorASucesores);
    vector<int> ready = InicializarReadyQueue(T, asignado, predPendientes);
    
    cout << "\033[95m=========================================\033[0m" << endl;
    cout << "\033[95m            TAREAS EN LA COLA            \033[0m" << endl;
    cout << "\033[95m=========================================\033[0m" << endl;
    
    MostrarReadyQueue(ready, tareas);
    
    // Encontrar el máximo procesador MIST (una sola vez al inicio)
    int maxMistProcessor = 0;
    for (const auto& t : tareas) {
        if (t.ProcAss > maxMistProcessor) {
            maxMistProcessor = t.ProcAss;
        }
    }
    
    // 5. BUCLE GREEDY PRINCIPAL
    while (!ready.empty()) {
        cout << "\033[95m=========================================\033[0m" << endl;
        cout << "\033[95m     SELECCION DEL MEJOR PROCESADOR      \033[0m" << endl;
        cout << "\033[95m=========================================\033[0m" << endl;

        // Ordenar por deadline más corto
        sort(ready.begin(), ready.end(),
             [&](int a, int b) { return tareas[a].D < tareas[b].D; });
        
        int tarea_actual = ready.front();
        ready.erase(ready.begin());
        
        // ============================================================
        // DETERMINAR PROCESADORES A CONSIDERAR (CON RESTRICCIONES MIST)
        // ============================================================
        vector<int> procesadoresAConsiderar;
        
        if (tareas[tarea_actual].ProcAss > 0) {
            // Tarea MIST: solo puede ir a su procesador asignado
            procesadoresAConsiderar.push_back(tareas[tarea_actual].ProcAss);
            cout << "  [MIST] Solo puede ir al P" << tareas[tarea_actual].ProcAss << endl;
        } else {
            // Tarea NO MIST: puede ir a cualquier procesador NO MIST
            for (const auto& p : procesadores) {
                if (p.id > maxMistProcessor) {  // Solo procesadores NO MIST
                    procesadoresAConsiderar.push_back(p.id);
                }
            }
            cout << "  Evaluando procesadores NO MIST (P" << (maxMistProcessor + 1) 
                 << " a P" << procesadores.size() << ")" << endl;
        }
        // ============================================================
        
        // Seleccionar mejor procesador
        auto [mejorProcesador, inicio_tarea] = SeleccionarMejorProcesador(tarea_actual, tareas, procesadoresAConsiderar,
                                                                         memLibre, uLibre, tiempoDisponible,
                                                                         sucesorAPredecesores, asignado,
                                                                         servidor, f, Delta);
        
        // Verificar si se encontró procesador válido
        if (mejorProcesador == -1) {
            cout << "\033[91m[ERROR] T" << tarea_actual << " no asignable a ningún procesador\033[0m" << endl;
            return {{}, {}, {}, false};
        }
        
        // Calcular inicio y fin
        int fin_tarea = inicio_tarea + tareas[tarea_actual].C;
        
        // Asignar la tarea
        asignado[tarea_actual] = true;
        servidor[tarea_actual] = mejorProcesador;
        s[tarea_actual] = inicio_tarea;
        f[tarea_actual] = fin_tarea;
        
        // Actualizar recursos
        ActualizarRecursos(tarea_actual, mejorProcesador, tareas[tarea_actual], 
                          tiempoDisponible, memLibre, uLibre, inicio_tarea, fin_tarea);
        
        cout << "\033[92m[ASIGNADO] T" << tarea_actual << " -> P" << mejorProcesador 
             << " | Inicio: " << inicio_tarea << " | Fin: " << fin_tarea << "\033[0m" << endl;
        
        // Actualizar ready queue
        ActualizarReadyQueue(tarea_actual, predecesorASucesores, predPendientes, asignado, ready, tareas);
        
        cout << "\033[95m=========================================\033[0m" << endl;
        cout << "\033[95m            TAREAS EN LA COLA            \033[0m" << endl;
        cout << "\033[95m=========================================\033[0m" << endl;
        
        MostrarReadyQueue(ready, tareas);
    }
    
    // 6. VERIFICAR ASIGNACIÓN COMPLETA
    for (int i = 0; i < T; i++) {
        if (!asignado[i]) {
            cout << "\033[91m[ERROR] T" << i << " no fue asignada\033[0m" << endl;
            return {{}, {}, {}, false};
        }
    }
    
    // 7. VERIFICAR DEADLINES Y MOSTRAR RESULTADO
    bool todasCumplen = VerificarDeadlines(T, tareas, f);
    MostrarAsignacionFinal(T, tareas, servidor, s, f);
    
    return {servidor, s, f, todasCumplen};
}