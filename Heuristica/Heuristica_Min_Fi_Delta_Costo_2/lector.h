#ifndef LECTOR_H
#define LECTOR_H

#include <string>
#include <vector>
#include "datos.h"

bool leerDatos(const std::string& filename,
               std::vector<Procesador>& procesadores,
               std::vector<Tarea>& tareas,
               std::vector<Precedencia>& precedencias,
               std::vector<Comunicacion>& comunicaciones);

#endif
