<div align="center">

# 🇵🇰 PakTour Planner

### Smart Detour Recommendation Engine & Custom Itinerary Builder

**A Data Structures & Algorithms Semester Project**

[![C++](https://img.shields.io/badge/C%2B%2B-17-00599C?style=flat&logo=c%2B%2B&logoColor=white)](https://en.cppreference.com/w/cpp/17)
[![CMake](https://img.shields.io/badge/Build-CMake-064F8C?style=flat&logo=cmake&logoColor=white)](https://cmake.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-lightgrey)]()
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

*A graph-based route optimization engine that finds the best travel path between Pakistani tourist destinations — by cost, by time, or both.*

</div>

---

## 📖 Table of Contents

<details>
<summary>Click to expand</summary>

- [Overview](#-overview)
- [Core Algorithm](#-core-algorithm)
- [Features](#-features)
- [Project Architecture](#-project-architecture)
- [Complexity Analysis](#-complexity-analysis)
- [Getting Started](#-getting-started)
- [Usage](#-usage)
- [Data Format](#-data-format)
- [Project Structure](#-project-structure)
- [Design Decisions](#-design-decisions)
- [Roadmap](#-roadmap)
- [Academic Context](#-academic-context)
- [License](#-license)

</details>

---

## 🧭 Overview

**PakTour Planner** models Pakistan's tourist destinations as a **weighted graph**, where cities are vertices and roads/routes are edges carrying two independent weights — **cost** and **time**. The engine applies **Dijkstra's shortest-path algorithm** to compute the optimal route between any two cities based on the traveler's chosen priority.

Built as a semester project for a Data Structures & Algorithms course, this project demonstrates a practical, real-world application of graph theory, priority queues, and greedy algorithms.

---

## ⚙️ Core Algorithm

```
┌─────────────────────────────────────────────────────────┐
│                     DIJKSTRA'S ALGORITHM                │
├─────────────────────────────────────────────────────────┤
│                                                           │
│   Graph G = (V, E)                                       │
│   V = { Islamabad, Lahore, Skardu, Hunza, Swat, ... }    │
│   E = { (u, v, cost, time) }                             │
│                                                           │
│   1. Initialize dist[source] = 0, all others = ∞         │
│   2. Push source into min-heap priority queue            │
│   3. While heap is not empty:                            │
│        - Pop city u with smallest tentative distance     │
│        - Relax all edges (u, v):                         │
│              if dist[u] + weight(u,v) < dist[v]:         │
│                  dist[v] = dist[u] + weight(u,v)         │
│                  push v into heap                        │
│   4. Reconstruct path via predecessor tracking           │
│                                                           │
└─────────────────────────────────────────────────────────┘
```

The graph supports **dual-metric optimization** — the same underlying algorithm runs against either the `cost` weight or the `time` weight depending on user preference, without duplicating logic.

---

## ✨ Features

| Feature | Description |
|---|---|
| 🗺️ **Weighted Graph Model** | Cities and roads represented via an adjacency list |
| ⚖️ **Dual-Weight Optimization** | Choose the shortest route by **cost** or by **time** |
| 🚀 **Min-Heap Priority Queue** | Efficient `O((V + E) log V)` implementation of Dijkstra |
| 🌦️ **Seasonal Tagging** | Every city tagged `SUMMER`, `WINTER`, or `BOTH` to filter unreachable/inadvisable destinations by season |
| 🧩 **Modular Design** | Clean separation of concerns: `City`, `Road`, `Graph`, `Dijkstra`, `AttractionManager` |
| 📄 **CSV-Driven Data** | Destinations and routes loaded from external `data/*.csv` files — no hardcoded data |
| 🔧 **CMake Build System** | Cross-platform build configuration (Windows/Linux) |
| 🎯 *(Stretch Goal)* **Normalized Combined-Weight Formula** | Blend cost and time into a single weighted score for "balanced" route suggestions |

---

## 🏗️ Project Architecture

```
                     ┌──────────────┐
                     │   main.cpp   │
                     │ (Entry Point)│
                     └──────┬───────┘
                            │
              ┌─────────────┼─────────────┐
              ▼             ▼             ▼
        ┌──────────┐  ┌──────────┐  ┌─────────────────────┐
        │  Graph   │  │ Dijkstra │  │ AttractionManager    │
        │ (Model)  │◄─┤ (Engine) │  │ (Seasonal filtering, │
        └────┬─────┘  └──────────┘  │  city metadata)      │
             │                      └─────────────────────┘
     ┌───────┴────────┐
     ▼                ▼
┌─────────┐      ┌─────────┐
│  City   │      │  Road   │
│ (Vertex)│      │ (Edge)  │
└─────────┘      └─────────┘
```

- **`City`** — represents a vertex: name, region, and season tag (`SUMMER` / `WINTER` / `BOTH`)
- **`Road`** — represents a weighted edge: source, destination, cost, and time
- **`Graph`** — adjacency-list container that owns all cities and roads
- **`Dijkstra`** — the shortest-path engine, parameterized by weight type (cost/time)
- **`AttractionManager`** — handles seasonal filtering and destination metadata

---

## 📊 Complexity Analysis

| Operation | Complexity | Notes |
|---|---|---|
| Graph construction | `O(V + E)` | Reading cities and roads from CSV |
| Dijkstra (min-heap) | `O((V + E) log V)` | Binary heap–based priority queue |
| Space complexity | `O(V + E)` | Adjacency list representation |
| Seasonal filter pass | `O(V)` | Applied before/after pathfinding |

> Using an adjacency list instead of an adjacency matrix keeps the graph sparse-friendly — appropriate since not every city in Pakistan has a direct route to every other city.

---

## 🚀 Getting Started

### Prerequisites

- CMake ≥ 3.16
- A C++17-compatible compiler (MSVC / GCC / Clang)

### Build (Windows — Visual Studio)

```bash
# Open the folder in Visual Studio as a CMake project
# Visual Studio will auto-detect CMakeLists.txt via launch.vs.json
# Build using Ctrl+Shift+B, then run PakTourPlanner.exe
```

### Build (Linux / macOS / CLI)

```bash
git clone https://github.com/<your-username>/PakTourPlanner.git
cd PakTourPlanner

mkdir build && cd build
cmake ..
cmake --build .

./PakTourPlanner
```

The build system automatically copies `data/*.csv` into the output directory, so the executable can locate destination data at runtime.

---

## 💻 Usage

```
$ ./PakTourPlanner

=======================================
     PakTour Planner — Route Finder
=======================================
Enter source city      : Islamabad
Enter destination city : Hunza
Optimize by [1] Cost  [2] Time : 2

Calculating optimal route...

Route Found:
Islamabad → Abbottabad → Mansehra → Besham → Hunza

Total Time   : 11.5 hrs
Total Cost   : PKR 4,200
Season Check : ✅ Reachable (Route valid in current season)
```

> Sample output — adjust to match your actual CLI once finalized.

---

## 📂 Data Format

### `data/cities.csv`

| Field | Type | Description |
|---|---|---|
| `city_id` | int | Unique identifier |
| `name` | string | City name |
| `region` | string | Province/region |
| `season_tag` | enum | `SUMMER`, `WINTER`, or `BOTH` |

```csv
city_id,name,region,season_tag
1,Islamabad,Punjab,BOTH
2,Hunza,Gilgit-Baltistan,SUMMER
3,Murree,Punjab,WINTER
```

### `data/roads.csv`

| Field | Type | Description |
|---|---|---|
| `from_id` | int | Source city ID |
| `to_id` | int | Destination city ID |
| `cost` | double | Travel cost (PKR) |
| `time` | double | Travel time (hours) |

```csv
from_id,to_id,cost,time
1,2,4200,11.5
1,3,1500,2.5
```

---

## 📁 Project Structure

```
PakTourPlanner/
├── CMakeLists.txt
├── launch.vs.json
├── data/
│   ├── cities.csv
│   └── roads.csv
├── include/
│   ├── City.h
│   ├── Road.h
│   ├── Graph.h
│   ├── Dijkstra.h
│   └── AttractionManager.h
└── src/
    ├── City.cpp
    ├── Road.cpp
    ├── Graph.cpp
    ├── Dijkstra.cpp
    ├── AttractionManager.cpp
    └── main.cpp
```

---

## 🎯 Design Decisions

- **Two-weight system** (cost & time) instead of a single generic weight, since real travelers optimize for different priorities.
- **Adjacency list + min-heap** chosen over adjacency matrix + linear scan for better performance on sparse, real-world road networks.
- **Season tagging** added because several northern Pakistani destinations (e.g., high-altitude valleys) are seasonally inaccessible — a purely graph-theoretic shortest path isn't enough without this real-world constraint layer.
- **Stretch goal:** a normalized combined-weight formula (e.g., `α·cost_norm + (1−α)·time_norm`) to support a single "balanced" route mode.

---

## 🗺️ Roadmap

- [x] Core graph model (`City`, `Road`, `Graph`)
- [x] Dijkstra's algorithm with min-heap priority queue
- [x] Seasonal filtering via `AttractionManager`
- [x] CSV-based data loading
- [ ] Normalized combined cost/time weighting
- [ ] Multi-destination itinerary builder (TSP-style extension)
- [ ] Simple visualization of the route graph

---

## 🎓 Academic Context

This project was developed as a semester project for a **Data Structures & Algorithms** course, applying:

- Graph representations (adjacency list)
- Priority queues / binary heaps
- Greedy algorithms (Dijkstra's shortest path)
- Object-oriented design in C++

---

## 📜 License

This project is licensed under the MIT License — see the [LICENSE](LICENSE) file for details.

---

<div align="center">

Made with ❤️ for Pakistan's tourism, one shortest path at a time.

</div>
