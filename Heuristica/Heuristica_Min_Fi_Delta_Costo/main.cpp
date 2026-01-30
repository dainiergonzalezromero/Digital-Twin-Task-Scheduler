#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cstdlib>  // Para atof
#include <cstring>  // Para strcmp
#include <fstream>  // Para archivos de salida

#include "datos.h"
#include "lector.h"
#include "visualizador.h"
#include "heuristica.h"

using namespace std;

// Función para extraer el valor de un parámetro
double extraerValorParametro(const string& param) {
    size_t pos = param.find('=');
    if (pos != string::npos) {
        return atof(param.substr(pos + 1).c_str());
    }
    return 1.0; // Valor por defecto si no se encuentra '='
}

// Función para extraer valor booleano
bool extraerValorBooleano(const string& param) {
    size_t pos = param.find('=');
    if (pos != string::npos) {
        string valor = param.substr(pos + 1);
        // Convertir a minúsculas y comparar
        transform(valor.begin(), valor.end(), valor.begin(), ::tolower);
        return (valor == "true" || valor == "1" || valor == "yes");
    }
    return false;
}

// Función para extraer string de parámetro
string extraerStringParametro(const string& param) {
    size_t pos = param.find('=');
    if (pos != string::npos) {
        return param.substr(pos + 1);
    }
    return "";
}

// Función para mostrar resultados en formato simple (CSV-like) en pantalla
void mostrarResultadosSimple(const vector<int>& servidor, const vector<int>& s, const vector<int>& f) {
    cout << "Servidor,Inicio,Fin" << endl;
    for (size_t i = 0; i < servidor.size(); i++) {
        cout  << servidor[i] << "," << s[i] << "," << f[i] << endl;
    }
}

// Función para generar archivo CSV
void generarArchivoCSV(const vector<int>& servidor, const vector<int>& s, const vector<int>& f, 
                      const string& nombreArchivo) {
    ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cerr << "Error al crear archivo: " << nombreArchivo << endl;
        return;
    }
    
    // Encabezados
    archivo << "Tarea,Servidor,Inicio,Fin" << endl;
    
    // Datos
    for (size_t i = 0; i < servidor.size(); i++) {
        archivo << i << "," << servidor[i] << "," << s[i] << "," << f[i] << endl;
    }
    
    archivo.close();
    cout << "Resultados exportados a: " << nombreArchivo << endl;
}

// Función para mostrar ayuda
void mostrarAyuda(const char* nombrePrograma) {
    cout << "=========================================\n";
    cout << "   PROGRAMADOR DE TAREAS (SCHEDULER)\n";
    cout << "=========================================\n\n";
    cout << "Uso: " << nombrePrograma << " <archivo_instancia> [OPCIONES]\n\n";
    cout << "OPCIONES:\n";
    cout << "  --alfa=<valor>    Peso para tiempo de finalización (default: 1.0)\n";
    cout << "  --beta=<valor>    Peso para delays de comunicación (default: 1.0)\n";
    cout << "  --gamma=<valor>   Peso para costos de procesador (default: 1.0)\n";
    cout << "  --debug=<true/false>  Modo de depuración (default: false)\n";
    cout << "  --csv                    Mostrar resultados en formato CSV simple en pantalla\n";
    cout << "  --output=<nombre>, -o=<nombre>  Exportar resultados a archivo CSV\n";
    cout << "  --output=true/false             Activar/desactivar salida CSV\n";
    cout << "  --help, -h               Muestra esta ayuda\n\n";
    cout << "EJEMPLOS:\n";
    cout << "  " << nombrePrograma << " instancia.dat\n";
    cout << "  " << nombrePrograma << " instancia.dat --alfa=1.0 --beta=0.5 --gamma=2.0\n";
    cout << "  " << nombrePrograma << " instancia.dat --debug=true --alfa=0.8\n";
    cout << "  " << nombrePrograma << " instancia.dat --csv\n";
    cout << "  " << nombrePrograma << " instancia.dat --output=resultados.csv\n";
    cout << "  " << nombrePrograma << " instancia.dat -o=salida.csv\n";
    cout << "  " << nombrePrograma << " instancia.dat --output=true  # Crea 'salida.csv'\n";
    cout << "=========================================\n";
}

int main(int argc, char* argv[]) {
    // Verificar argumentos mínimos
    if (argc < 2) {
        cerr << "Error: Se requiere archivo de instancia\n\n";
        mostrarAyuda(argv[0]);
        return 1;
    }
    
    vector<Procesador> procesadores;
    vector<Tarea> tareas;
    vector<Precedencia> precedencias;
    vector<Comunicacion> comunicaciones;
    
    // Valores por defecto
    double alfa = 1.0;   // Peso para tiempo de finalización
    double beta = 1.0;   // Peso para delays de comunicación
    double gamma = 1.0;  // Peso para costos de procesador
    bool debug = false;  // Modo debug
    bool formatoCSV = false; // Mostrar en formato CSV simple en pantalla
    bool generarCSV = false; // Generar archivo CSV
    string nombreArchivoCSV = "salida.csv"; // Nombre por defecto para archivo CSV
    
    // Procesar argumentos
    string archivoInstancia = argv[1];
    
    for (int i = 2; i < argc; i++) {
        string arg = argv[i];
        
        if (arg.find("--alfa=") == 0) {
            alfa = extraerValorParametro(arg);
        } 
        else if (arg.find("--beta=") == 0) {
            beta = extraerValorParametro(arg);
        } 
        else if (arg.find("--gamma=") == 0) {
            gamma = extraerValorParametro(arg);
        } 
        else if (arg.find("--debug=") == 0) {
            debug = extraerValorBooleano(arg);
        }
        else if (arg == "--csv") {
            formatoCSV = true;
        }
        else if (arg.find("--output=") == 0 || arg.find("-o=") == 0) {
            string valor = extraerStringParametro(arg);
            
            // Verificar si es true/false o un nombre de archivo
            transform(valor.begin(), valor.end(), valor.begin(), ::tolower);
            if (valor == "true" || valor == "1" || valor == "yes") {
                generarCSV = true;
                // Usar nombre por defecto
            } else if (valor == "false" || valor == "0" || valor == "no") {
                generarCSV = false;
            } else if (!valor.empty()) {
                // Es un nombre de archivo
                generarCSV = true;
                nombreArchivoCSV = valor;
            }
        }
        else if (arg == "--help" || arg == "-h") {
            mostrarAyuda(argv[0]);
            return 0;
        }
        else {
            cerr << "Advertencia: Parámetro desconocido '" << arg << "' ignorado\n";
            cerr << "Use --help para ver las opciones disponibles\n";
        }
    }
    
    // Validar parámetros
    if (alfa < 0 || beta < 0 || gamma < 0) {
        cerr << "Error: Los pesos (alfa, beta, gamma) deben ser no negativos\n";
        return 1;
    }
    
    // Cargar datos
    if (debug && !formatoCSV) {
        cout << "[DEBUG] Cargando datos del archivo: " << archivoInstancia << endl;
    }

    if (!leerDatos(archivoInstancia.c_str(), procesadores, tareas, precedencias, comunicaciones)) {
        cerr << "Error al leer archivo\n";
        return 1;
    }
    
    // Ejecutar heurística
    auto resultado = heuristicaGreedy(
        procesadores, tareas, precedencias, comunicaciones, alfa, beta, gamma, debug
    );

    if (!resultado.factible) {
        if (formatoCSV) {
            cout << "ERROR,Solución NO factible" << endl;
        } else if (generarCSV) {
            ofstream archivo(nombreArchivoCSV);
            if (archivo.is_open()) {
                archivo << "ERROR,Solución NO factible" << endl;
                archivo.close();
            }
            cout << "Solución NO factible - Resultados en: " << nombreArchivoCSV << endl;
        } else {
            cout << "\n❌ Solución NO factible\n";
        }
        return 0;
    }
  
    // Opciones de salida
    if (formatoCSV) {
        // Mostrar en pantalla en formato CSV simple
        mostrarResultadosSimple(resultado.servidor, resultado.s, resultado.f);
    } 
    else if (generarCSV) {
        // Generar archivo CSV
        generarArchivoCSV(resultado.servidor, resultado.s, resultado.f, nombreArchivoCSV);
        
        // También mostrar resumen breve en pantalla
        cout << "\n✅ SOLUCIÓN HEURÍSTICA GENERADA\n";
        cout << "Resultados exportados a: " << nombreArchivoCSV << endl;
    }
    else {
        // Formato normal (detallado)
        cout << "=========================================\n";
        cout << "CONFIGURACIÓN DE PARÁMETROS:\n";
        cout << "=========================================\n";
        cout << "Archivo de instancia: " << archivoInstancia << endl;
        cout << "Alfa  (peso tiempo finalización): " << alfa << endl;
        cout << "Beta  (peso delays comunicación): " << beta << endl;
        cout << "Gamma (peso costos procesador):   " << gamma << endl;
        cout << "Modo debug:                       " << (debug ? "Activado" : "Desactivado") << endl;
        cout << "=========================================\n\n";
        
        // Calcular estadísticas
        int suma_fi = 0;
        int suma_costo = 0;
        
        cout << "\n✅ SOLUCIÓN HEURÍSTICA\n";
        cout << "Parámetros: Alfa=" << alfa << ", Beta=" << beta << ", Gamma=" << gamma << "\n";
        cout << "\nTAREAS ASIGNADAS:\n";
        cout << "==================================================================\n";
        
        for (size_t i = 0; i < tareas.size(); i++) {
            cout << "Task " << i << ": "
                 << "Server P" << resultado.servidor[i]
                 << " | Start: " << resultado.s[i]
                 << " | Finish: " << resultado.f[i] 
                 << " | Cost/unit: " << resultado.cost[i] << "\n";
            suma_fi += resultado.f[i];
            suma_costo += resultado.cost[i];
        }

        // Calcular valor de la función objetivo completa
        double valor_objetivo_total = alfa * suma_fi + 
                                      beta * resultado.total_delays + 
                                      gamma * suma_costo;
        
        cout << "\n=========================================\n";
        cout << "   COMPONENTES DE LA FUNCIÓN OBJETIVO:\n";
        cout << "=========================================\n";
        cout << "Alfa * Σf_i = " << alfa << " * " << suma_fi << " = " << (alfa * suma_fi) << endl;
        cout << "Beta * Σdelays = " << beta << " * " << resultado.total_delays << " = " << (beta * resultado.total_delays) << endl;
        cout << "Gamma * ΣCost_s = " << gamma << " * " << suma_costo << " = " << (gamma * suma_costo) << endl;
        
        cout << "\nVALOR TOTAL DE LA FUNCIÓN OBJETIVO:\n";
        cout << valor_objetivo_total << endl;
        cout << "=========================================\n";
    }

    return 0;
}
