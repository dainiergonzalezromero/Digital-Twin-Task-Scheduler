# =====================================================
# DIGITAL TWIN – Task Allocation and Scheduling
# =====================================================


# =========================
# CONJUNTOS
# =========================

param S;                     # Número de tareas |S|
param P;                     # Número de servidores |P|

set Tasks   := 0..S-1;        # Conjunto de tareas τ_i
set Servers := 1..P;          # Conjunto de servidores P_s


# =========================
# PARÁMETROS DE TIEMPO
# =========================

# Tiempo discreto implícito (t ∈ N ∪ {0})
# El paper fija r_i = 0
param r {i in Tasks} = 0;     # Release time r_i


# =========================
# PARÁMETROS DE TAREAS
# =========================

param C  {i in Tasks} >= 0;   # WCET C_i
param T  {i in Tasks} >  0;   # Periodo T_i
param D  {i in Tasks} >= 0;   # Deadline relativo D_i
param ac {i in Tasks} >= 0;   # Tiempo de activación ac_i
param MemTask {i in Tasks} >= 0;   # Memoria requerida M_i

# Utilización (definida como en el paper)
param u {i in Tasks} := C[i] / T[i];


# =========================
# PARÁMETROS DE SERVIDORES
# =========================

param M {srv in Servers} >= 0;  # Memoria disponible MEM_s
param U {srv in Servers} >= 0;  # Capacidad de CPU U_s
param COST{srv in Servers} >= 0; # Costo de Uso del Servidor


# =========================
# ASIGNACIÓN FIJA DE TAREAS
# =========================

# 0  -> tarea libre
# >0 -> tarea forzada al servidor indicado
param ServidorFijo {i in Tasks} default 0;


# =========================
# PRECEDENCIAS (DAG)
# =========================

# τ_h ≺ τ_i
set PRECEDENCES within {i in Tasks, j in Tasks};


# =========================
# DELAYS DE COMUNICACIÓN
# =========================

# Δ_uv = delay entre servidores u y v
# Δ_uu = 0, Δ_uv = Δ_vu
param Delta {uu in Servers, vu in Servers} >= 0;


# =========================
# VARIABLES DE DECISIÓN
# =========================

# Asignación tarea–servidor
var L {i in Tasks, srv in Servers} binary;

# Tiempos de ejecución
var s {i in Tasks} integer >= 0;   # tiempo de inicio
var f {i in Tasks} integer >= 0;   # tiempo de finalización

# Orden de ejecución en un servidor
var delta {i in Tasks, j in Tasks} binary;

# Variable auxiliar precedencia + asignación
var psi {i in Tasks, j in Tasks, uu in Servers, vu in Servers} binary;


# =========================
# PARÁMETROS AUXILIARES
# =========================

param alpha >= 0;
param beta  >= 0;
param gamma >= 0;
param BigM  >  0;


# =========================
# FUNCIÓN OBJETIVO
# =========================

minimize Objective_Function:
    alpha * sum {i in Tasks} f[i] + beta  * sum {(h,i) in PRECEDENCES, uu in Servers, vu in Servers} psi[h,i,uu,vu] * Delta[uu,vu] + gamma * sum{i in Tasks, srv in Servers} COST[srv] * L[i,srv];
    
# =========================
# RESTRICCIONES
# =========================

# Deadline
s.t. DeadlineConstraint {i in Tasks}:
    f[i] <= r[i] + D[i];

# Relación inicio–fin
s.t. ExecutionTime {i in Tasks}:
    f[i] = s[i] + C[i];

# Capacidad de CPU
s.t. CPUCapacity {srv in Servers}:
    sum {i in Tasks} L[i,srv] * u[i] <= U[srv];

# Capacidad de memoria
s.t. MEMCapacity {srv in Servers}:
    sum {i in Tasks} L[i,srv] * MemTask[i] <= M[srv];

# Cada tarea en un solo servidor
s.t. AssignmentOne {i in Tasks}:
    sum {srv in Servers} L[i,srv] = 1;

# Orden de ejecución (Big-M)
s.t. Ordering {i in Tasks, j in Tasks: i <> j}:
    s[i] >= f[j] - BigM * (1 - delta[i,j]);

# Activación del orden solo si comparten servidor
s.t. OrderActivation {i in Tasks, h in Tasks, srv in Servers: i <> h}:
    L[i,srv] + L[h,srv] - 1 <= delta[i,h] + delta[h,i];

# Un único par de servidores por precedencia
s.t. PsiUnique {(h,i) in PRECEDENCES}:
    sum {uu in Servers, vu in Servers} psi[h,i,uu,vu] <= 1;

# Activación de psi
s.t. PsiOne {(h,i) in PRECEDENCES, uu in Servers, vu in Servers}:
    psi[h,i,uu,vu] <= L[h,uu];

s.t. PsiTwo {(h,i) in PRECEDENCES, uu in Servers, vu in Servers}:
    psi[h,i,uu,vu] <= L[i,vu];

s.t. PsiThree {(h,i) in PRECEDENCES, uu in Servers, vu in Servers}:
    psi[h,i,uu,vu] >= L[h,uu] + L[i,vu] - 1;

# Precedencia con delay de comunicación
s.t. PrecedenceDelay {(h,i) in PRECEDENCES,
                      uu in Servers, vu in Servers}:
    s[i] >= f[h] + Delta[uu,vu] - BigM * (1 - psi[h,i,uu,vu]);

# Precedencia básica (sin delay)
s.t. PrecedenceBasic {(h,i) in PRECEDENCES}:
    s[i] >= f[h];

# Servidores fijos
s.t. FixedServer {i in Tasks: ServidorFijo[i] != 0}:
    L[i, ServidorFijo[i]] = 1;
