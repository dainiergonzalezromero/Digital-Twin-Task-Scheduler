# Digital Twin Task Allocation and Scheduling

This repository contains the implementation of **exact and heuristic methods** for task allocation and scheduling in a **Digital Twin architecture over a Mist–Edge–Fog-Cloud continuum**, considering **precedence constraints, communication delays, resource limitations, and processing costs**.

## 📌 Problem Overview

We address the problem of allocating and scheduling a set of interdependent real-time tasks modeled as a **Directed Acyclic Graph (DAG)** onto a heterogeneous set of computational resources distributed across **Mist, Edge,Fog and Cloud layers**.

Each task is characterized by:
- Worst-Case Execution Time (WCET)
- Period
- Deadline
- Release time
- Memory requirement
- Optional fixed assignment to a Mist node

Each server is characterized by:
- CPU capacity
- Available memory
- Usage cost
- Communication delays with other servers

The goal is to determine:
1. **Task-to-server allocation**
2. **Execution order and timing**

while minimizing a multi-objective cost function under strict constraints.

---

## 🎯 Optimization Objective

The problem is formulated as a **Mixed-Integer Linear Program (MILP)** with the following objective: 

$$
\min \ \Bigg(
\alpha \sum_{i} f_{i}
\+\
\beta \sum_{h,i,u,v} \Delta_{uv}\,\psi_{hiuv}
\+\
\gamma \sum_{s,i} \mathrm{Cost}_{s}\,L_{is}
\Bigg)
$$

Subject to:

- **Deadline constraints** (for all tasks i):`f[i] <=  r[i] + D[i]`
- **Non-preemptive execution** (for all tasks i): `f[i] = s[i] + C[i]`
- **CPU capacity** (for all servers s):`∑L[i,s]*u[i] <= U[s]`
- **Memory capacity** (for all servers s): `∑[i] L[i,s] * M[i] ≤ MEM[s]`
- **Precedence with communication delays** (for every precedence h → i and processors u,v): `s[i] ≥ f[h] + Δ[u,v]` if `τ[h] ≺ τ[i]` and `L[h,u]=1`, `L[i,v]=1`
- **Task assignment uniqueness** (each task assigned to exactly one server): `∑[s] L[i,s] = 1` ∀ i
- **Fixed assignments to Mist** (if task i is fixed to server s₀): `L[i,s₀] = 1`
- **Variable domains**: `L[i,s] ∈ {0,1}, s[i] ≥ 0, f[i] ≥ 0, ψ[h,i,u,v] ≥ 0`

A **greedy heuristic** is also proposed to efficiently approximate the MILP solution for larger instances.
---

## 📁 Repository Structure
```
├── AMPL/
│   ├── servers_and_tasks.mod        # MILP model (AMPL)
│   ├── datos_org.dat                # Example instance
│   └── resol.run                    # AMPL run file
│
├── SCIP/
│   ├── modelo.zpl                   # MILP model in ZIMPL (SCIP)
│   ├── generada.dat                 # Instance file
│   └── paralelo.set                 # SCIP solver configuration
│
├── Generar_Datos/
│   ├── generate.py                  # Synthetic instance generator
│   ├── Leer_Graficar.py             # Instance visualization
│   └── dat/                         # Generated instances
│
├── Heuristica/
│   ├── Heuristica Min Fi/
│   │   ├── main.cpp
│   │   ├── heuristica.cpp
│   │   ├── lector.cpp
│   │   └── visualizador.cpp
│   │
│   └── Heuristica Min Fi + Costo/
│       ├── main.cpp
│       ├── heuristica.cpp
│       ├── datos.h
│       ├── lector.cpp
│       ├── visualizador.cpp
│       └── instance_*.dat
```

## 🧪 Instance Generation

Synthetic task graphs and system instances are generated using the scripts provided in the `Generar_Datos/` directory.

### Generator

```bash
cd Generar_Datos
python3 generate.py --Tasks=T --Sensors=S --Actuators=A --Edge=E --Fog-F
```
The generator creates:
- Task sets with WCET, deadlines, memory requirements
- Directed Acyclic Graph (DAG) precedence relations
- Heterogeneous processors (Mist / Edge /Fog / Cloud)
- Inter-processor communication delays

Generated instances are stored in Generar_Datos/dat/ and can be used for:

* AMPL (.dat)
* SCIP/ZIMPL (.dat)
* Heuristic execution (plain text format)
---

## 🧮 MILP Model (AMPL)

The exact formulation is implemented in **AMPL** and solved using **Gurobi**.

### Decision Variables
- `L[i,s]`: Binary variable indicating assignment of task *i* to server *s*
- `s[i], f[i]`: Start and finish times of task *i*
- `delta[i,j]`: Execution order between tasks
- `psi[h,i,u,v]`: Auxiliary variable for precedence and communication delay
- `COST[s]`: Cost of assigning a task to server *s*

### 🧮 Execution
```bash
ampl resol.run
```

---

## ⚙️ Heuristic Approach

A **greedy list-based heuristic** is implemented in C++ to approximate the MILP solution with lower computational cost.

### Key Features
- Topological scheduling using a ready queue
- Support for fixed Mist task assignments
- Explicit modeling of communication delays
- Resource feasibility checks (CPU and memory)
- Cost-aware processor selection

### Selection Strategy

Processors are selected using a **lexicographic criterion**:
1. Minimize task finishing time
2. Minimize server cost within a bounded time tolerance

### Compilation & Execution
Compile:

```bash
cd Heuristica/Heuristica\ Min\ Fi\ +\ Costo/
g++ -std=c++17 main.cpp heuristica.cpp lector.cpp visualizador.cpp -o scheduler
```
Run:

```bash
./scheduler instance_20x22.dat --alfa=1 --beta=0 --gamma=1 --debug=true 
```
---

## 💰 Cost Modeling

Each server has an associated usage cost.

In the heuristic:
- Each task inherits the cost of the server it is assigned to
- Cost is used as a secondary optimization criterion
- Total system cost is computed as the sum of task costs

This mirrors the cost term in the MILP formulation.

---

## ✅  SCIP / ZIMPL Model 

---

## 🧩 SCIP / ZIMPL Formulation
In addition to the AMPL implementation, the MILP model is also provided in **ZIMPL** format to be solved using **SCIP**.

### Files

- `modelo.zpl`: ZIMPL formulation of the task allocation and scheduling problem
- `generada.dat`: Input instance file
- `paralelo.set`: SCIP configuration for parallel execution

### Execution

```bash
scip -s paralelo.set -f modelo.zpl
```

## 🔗 Model Consistency

The three implementations provided in this repository are fully aligned:
- **AMPL (Exact MILP)**: Reference mathematical formulation
- **ZIMPL / SCIP**: Alternative exact solver implementation
- **C++ Heuristic**: Scalable approximation for large instances

All approaches share:
- The same task and processor parameters
- Identical precedence and communication models
- Consistent cost definitions

This ensures fair comparison between exact and heuristic solutions.

---


