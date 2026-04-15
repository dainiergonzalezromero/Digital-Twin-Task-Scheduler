# 🌐 Digital Twin Task Allocation and Scheduling

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)]()
[![Python](https://img.shields.io/badge/Python-3.8+-yellow.svg)]()
[![Optimization](https://img.shields.io/badge/Optimization-MILP-green.svg)]()


> **Exact and heuristic methods** for task allocation and scheduling in a **Digital Twin architecture** across the **Mist–Edge–Fog–Cloud continuum**.

This repository provides an optimization framework for allocating and scheduling interdependent real-time tasks modeled as **Directed Acyclic Graphs (DAGs)** onto heterogeneous computational resources. The approach considers **precedence constraints, communication delays, resource limitations, and processing costs**, enabling fair comparison between **exact MILP formulations** and **scalable heuristic solutions**.

---

## 📑 Table of Contents

- [📌 Problem Overview](#-problem-overview)
- [🏗️ System Architecture](#️-system-architecture)
- [✨ Key Features](#-key-features)
- [🚀 Quick Start](#-quick-start)
- [🛠️ Requirements](#️-requirements)
- [🎯 Optimization Objective](#-optimization-objective)
- [📁 Repository Structure](#-repository-structure)
- [🎲 Instance Generation](#-instance-generation)
- [⚡ Heuristic Approach (C++)](#-heuristic-approach-c)
- [🧮 Exact MILP (AMPL)](#-exact-milp-ampl)
- [🧩 Exact MILP (SCIP/ZIMPL)](#-exact-milp-scipzimpl)
- [📊 Example Output](#-example-output)
- [🧪 Reproducing Experiments](#-reproducing-experiments)
- [📊 Performance Comparison](#-performance-comparison)
- [🔗 Model Consistency](#-model-consistency)
- [📖 Citation](#-citation)
- [📄 License](#-license)
- [🤝 Contributing](#-contributing)
- [📧 Contact](#-contact)

---

## 📌 Problem Overview

We address the allocation and scheduling of interdependent real-time tasks onto computational resources distributed across **Mist, Edge, Fog, and Cloud layers**.

### 🧩 Task Characteristics
Each task *i* is defined by:
- **WCET** (Worst-Case Execution Time)
- **Period** and **Deadline**
- **Release time**
- **Memory requirement**
- **Optional fixed assignment** to a Mist node

### 🖥️ Server Characteristics
Each server *s* provides:
- **CPU capacity**
- **Available memory**
- **Usage cost**
- **Communication delays** with other servers

### 🎯 Decision Goals
1. **Task-to-server allocation**
2. **Execution order and timing**

---
## 🏗️ System Architecture

The system follows a hierarchical **Mist–Edge–Fog–Cloud continuum**:

| Layer | Characteristics |
|------|----------------|
| **Mist** | Local execution, ultra-low latency, limited resources |
| **Edge** | Near-device processing with moderate resources |
| **Fog** | Intermediate layer with balanced latency and capacity |
| **Cloud** | High computational power with higher latency and cost |


---

## ✨ Key Features

- ✅ **Multi-objective optimization** (completion time, communication delays, and costs)
- ✅ **Exact MILP formulations** using **AMPL** and **SCIP/ZIMPL**
- ✅ **Scalable greedy heuristic** implemented in **C++**
- ✅ **Synthetic instance generator** for benchmarking
- ✅ **Support for precedence and communication delays**
- ✅ **Resource-aware scheduling** (CPU and memory)
- ✅ **CSV export** for result analysis
- ✅ **Consistent modeling** across all implementations

---
---

## 🚀 Quick Start

```bash
# 1. Clone the repository
git clone https://github.com/dainiergonzalezromero/Digital-Twin-Task-Scheduler.git
cd Digital-Twin-Task-Scheduler

# 2. Generate a sample instance
cd Generar_Datos
python3 generate.py --Tasks=20 --Sensors=5 --Actuators=3 --Edge=2 --Fog=1 --Cloud=1

# 3. Compile and run the heuristic
cd ../Heuristica/Heuristica_Min_Fi_Delta_Costo
g++ -std=c++17 -O3 *.cpp -o scheduler
./scheduler ../../Generar_Datos/dat/datos.dat --t
```

---

## 🛠️ Requirements

### 🔧 Software

| Tool              | Purpose                    | Version  |
| ----------------- | -------------------------- | -------- |
| **Python**        | Instance generation        | ≥ 3.8    |
| **g++ / clang++** | Heuristic compilation      | C++17    |
| **AMPL**          | Exact MILP modeling        | Latest   |
| **Gurobi/CPLEX**  | MILP solver for AMPL       | Optional |
| **SCIP**          | Open-source solver         | ≥ 8.0    |
| **ZIMPL**         | Modeling language for SCIP | Latest   |

### 📦 Python Dependencies

```bash
pip install numpy pandas matplotlib networkx
```

---

## 🎯 Optimization Objective

The scheduling problem is formulated as a **Mixed-Integer Linear Program (MILP)** minimizing:

$$
\min \ \Bigg(
\alpha \sum_{i} f [i]
\+\
\beta \sum_{h,i,u,v} \Delta [u,v]\*\psi [h,i,u,v]
\+\
\gamma \sum_{s,i} \mathrm{Cost [s]*L [i,s]}
\Bigg)
$$

### Where:

* **α**: Weight for total completion time.
* **β**: Weight for communication delays.
* **γ**: Weight for server usage costs.
* **L[i,s]**: Binary assignment of task *i* to server *s*.
* **s[i], f[i]**: Start and finish times.

### 📌 Main Constraints

- **Deadline constraints** (for all tasks i):`f[i] <=  r[i] + D[i]`
- **Non-preemptive execution** (for all tasks i): `f[i] = s[i] + C[i]`
- **CPU capacity** (for all servers s):`∑L[i,s]*u[i] <= U[s]`
- **Memory capacity** (for all servers s): `∑[i] L[i,s] * M[i] ≤ MEM[s]`
- **Precedence with communication delays** (for every precedence h → i and processors u,v): `s[i] ≥ f[h] + Δ[u,v]` if `τ[h] ≺ τ[i]` and `L[h,u]=1`, `L[i,v]=1`
- **Task assignment uniqueness** (each task assigned to exactly one server): `∑[s] L[i,s] = 1` ∀ i
- **Fixed assignments to Mist** (if task i is fixed to server s₀): `L[i,s₀] = 1`
- **Variable domains**: `L[i,s] ∈ {0,1}, s[i] ≥ 0, f[i] ≥ 0, ψ[h,i,u,v] ≥ 0`

---

## 📁 Repository Structure
```
├── AMPL/
│ ├── servers_and_tasks.mod         # MILP model in AMPL
│ ├── resol.run                     # AMPL run script
│ └── datos2.dat                    # Example data instance
│
├── Generar_Datos/
│ ├── generate.py                   # Synthetic instance generator
│ ├── Leer_Graficar.py              # Instance visualization
│ └── dat/                          # Generated instances
│   ├── datos.dat
│   └── datos2.dat
│
├── Heuristica/
│ ├── dats/                         # Benchmark instances of various sizes
│ │ └── <size>dat_<i>.dat           # e.g., 10dat_1.dat, 100dat_2.dat, etc.
│ │
│ ├── Heuristica_Min_Fi_Delta_Costo/
│ │ ├── main.cpp                    # Entry point for heuristic version 1
│ │ ├── heuristica.cpp
│ │ ├── heuristica.h
│ │ ├── lector.cpp
│ │ ├── lector.h
│ │ ├── datos.h
│ │ ├── visualizador.cpp
│ │ ├── visualizador.h
│ │ └── scheduler                   # Compiled executable
│ │
│ ├── Heuristica_Min_Fi_Delta_Costo_2/
│ │ ├── main.cpp                    # Entry point for heuristic version 2
│ │ ├── heuristica.cpp
│ │ ├── heuristica.h
│ │ ├── lector.cpp
│ │ ├── lector.h
│ │ ├── datos.h
│ │ ├── visualizador.cpp
│ │ ├── visualizador.h
│ │ └── scheduler                   # Compiled executable
│ │
│ ├── leer_dat.py                   # Utility to read .dat files
│ ├── resumen.py                    # Results summarization
│ └── script.py                     # Automation script for experiments
│
├── SCIP/
│ ├── modelo.zpl                    # MILP model in ZIMPL for SCIP
│ ├── Evalua.sh                     # Script to execute experiments
│ ├── exacto                        # SCIP solver executable/configuration
│ └── dats/
│   └── datos.dat                   # Data instance for SCIP
│
└── readme.md                       # Project documentation

```

## 🎲 Instance Generation

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

### DAG Visualization

```bash
python3 Leer_Graficar.py dat/datos.dat
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
#### Compilation:

```bash
cd Heuristica/Heuristica_Min_Fi_Delta_Costo/
g++ -std=c++17 -O3 *.cpp -o scheduler
```
#### Execution:

Basic usage:

```bash
./scheduler <instance_file> [OPTIONS]
```

#### Examples:
```bash
# Standard execution: runs with default weights (alpha, beta, gamma = 1.0)
./scheduler instance.dat

# Selective optimization: ignores communication delays by setting beta to zero
./scheduler instance.dat --alpha=1 --beta=0 --gamma=1

# Development mode: enables verbose logging for algorithm debugging
./scheduler instance.dat --alpha=1 --beta=0 --gamma=1 --debug=true

# Production mode: suppresses debug logs (default behavior)
./scheduler instance.dat --alpha=1 --beta=0 --gamma=1 --debug=false

# Performance analysis: displays execution times, Fi summation, and cost metrics
./scheduler instance.dat --t

# Data formatting: outputs results in CSV format to the standard output (terminal)
./scheduler instance.dat --csv

# Data persistence: processes the instance and exports results to a specific file
./scheduler instance.dat --output=results.csv

# Short-hand alias: uses the '-o' flag to define the output destination (alias for --output)
./scheduler instance.dat -o=output.csv

# Quick export: enables CSV generation using the default filename ('output.csv')
./scheduler instance.dat --output=true  # Generates 'output.csv'

```

#### Available Options:

| Option                 | Description                     | Default |
| ---------------------- | ------------------------------- | ------- |
| `--alpha=<value>`      | Weight for completion time      | 1.0     |
| `--beta=<value>`       | Weight for communication delays | 1.0     |
| `--gamma=<value>`      | Weight for server costs         | 1.0     |
| `--t`                  | Display performance metrics     | false   |
| `--debug=<true/false>` | Enable verbose logging          | false   |
| `--csv`                | CSV output to stdout            | false   |
| `--output=<filename>`  | Export results to CSV           | -       |
| `--help, -h`           | Show help message               | -       |


---

## 💰 Cost Modeling

Each server has an associated usage cost.

In the heuristic:
- Each task inherits the cost of the server it is assigned to
- Cost is used as a secondary optimization criterion
- Total system cost is computed as the sum of task costs

This mirrors the cost term in the MILP formulation.

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
cd AMPL
ampl
include resol.run
```

## 🧩 SCIP / ZIMPL Formulation
---
In addition to the AMPL implementation, the MILP model is also provided in ZIMPL format to be solved using SCIP (or CPLEX).

### Files
- `modelo.zpl`: This is the core ZIMPL modeling file. It contains the mathematical formulation of the Mixed-Integer Linear Programming (MILP) model. It defines the sets, parameters (read from the data files), decision variables (such as task-to-processor mapping), and the multi-objective function that balances completion time, operational costs, and communication overhead.

- `exacto`: A solver command script. It sets the execution environment, including a time limit (18,000 seconds) and the MIP gap tolerance (0.5%). It automates the process of reading the generated .lp file, running the optimization, and exporting the results to resultado.sol.

- `Evalua.sh`: A Bash automation script that manages the entire workflow. It iterates through different test instances (from 10 to 50 tasks), prepares the data files, invokes ZIMPL to compile the model, and triggers the solver for each case.

- `dats/datos.dat`: A directory containing the specific problem instances. Each file includes the hardware parameters (memory and processor limits), task requirements (duration, costs), and the precedence matrices used to populate the model.

### Execution
To run the complete set of instances, ensure the script has execution permissions and run:
```bash
cd SCIP
chmod +x Evalua.sh
./Evalua.sh > salida.txt
```

##  Technical Workflow details
The execution follows this logical cycle for every instance:

1. **Data Preparation**: `Evalua.sh` takes a specific file from `dats/` and renames it to `instancia.dat`, which is the filename hardcoded in the ZIMPL model.

2. **Model Compilation**: The command `zimpl -o prueba modelo.zpl` is executed, translating the high-level ZIMPL code into a standard Linear Programming format (prueba.lp).

3. **Optimization**: The solver reads the `exacto` script, processes `prueba.lp`, and searches for the optimal solution.

4. **Logging**: All console outputs and solver logs are redirected to `salida.txt` for further analysis.

## Model Features (ZIMPL)
The model solves a Task Scheduling and Allocation problem with the following constraints:

- **Resource Allocation**: Checks for processor memory limits (`mem`) and utilization capacity (`util`).

- **Precedence & Latency**: Ensures tasks are executed in the correct order, accounting for communication delays between different processors (`precedence_start`).

- **Non-overlapping**: Uses Big-M constraints to ensure a single processor does not execute multiple tasks at the same time.

- **Multi-objective**: Minimizes a weighted sum of total completion time, processor costs, and inter-task communication costs.


---

## 📊 Example Output

```plaintext
Task  Server  Start  Finish  Cost
1     Edge1   0      5       2.5
2     Fog1    6      10      3.0
-----------------------------------
Total Completion Time: 45
Total Communication Cost: 12
Total System Cost: 35.5
```

---

## 🧪 Reproducing Experiments

```bash
cd Heuristica
python3 script.py
python3 resumen.py
```

---

## 📊 Performance Comparison

| Instance Size | Exact MILP | Heuristic | Speedup |
| ------------- | ---------- | --------- | ------- |
| 10 tasks      | < 1 sec    | < 0.1 sec | 10×     |
| 30 tasks      | 5–60 sec   | < 0.5 sec | 10–100× |
| 50 tasks      | 1–6 hours  | < 1 sec   | 1000×+  |
| 100 tasks     | > 24 hours | 2–5 sec   | 10000×+ |

*Performance depends on DAG density and constraint tightness.*

---

## 🔗 Model Consistency

All implementations share:

* Identical mathematical formulation
* Common data formats
* Consistent parameter definitions
* Comparable objective function components

This ensures **fair benchmarking** between exact and heuristic approaches.

---

## 📖 Citation

If you use this repository in your research, please cite:

```bibtex
@software{digital_twin_scheduling,
  author  = {Dainier González Romero},
  title   = {Digital Twin Task Allocation and Scheduling},
  year    = {2026},
  url     = {https://github.com/dainiergonzalezromero/Digital-Twin-Task-Scheduler/?tab=readme-ov-file#-example-output}
}
```

---

## 📄 License

This project is licensed under the **MIT License**. See the `LICENSE` file for details.

---

## 🤝 Contributing

Contributions are welcome! Please:

1. Fork the repository.
2. Create a feature branch.
3. Commit your changes.
4. Submit a pull request.

---

## 📧 Contact

For questions or collaborations, please open an issue in the repository or contact:

**Your Name**
📩 [dainier.gonzalez@cs.uns.edu.ar](mailto:dainier.gonzalez@cs.uns.edu.ar)

---

⭐ **If you find this repository useful, please consider giving it a star on GitHub!**

```
