/**
 * @file Dijkstra.cpp
 * @brief Implements the generic Dijkstra algorithm for PakTour Planner.
 *
 * A single findRoute() drives the algorithm for all three optimization
 * modes (Distance / TravelTime / FuelCost).  The mode affects only
 * which Road getter is used as the edge weight; every other data
 * structure and control-flow path is identical.
 *
 * Internal data structures:
 *   dist   : std::unordered_map<string, double>  – shortest known cost
 *   parent : std::unordered_map<string, string>   – predecessor in path
 *   pq     : std::priority_queue (min-heap via greater<>)
 *              each entry = { cost, cityName }
 *
 * Complexity: O((V + E) log V)
 *
 * PakTour Planner – Phase 2: Dijkstra's Algorithm
 */

#include "../include/Dijkstra.h"
#include "../include/Graph.h"
#include "../include/City.h"
#include "../include/Road.h"

#include <iostream>
#include <iomanip>
#include <queue>
#include <utility>      // std::pair
#include <functional>   // std::greater
#include <algorithm>    // std::reverse

// ---------------------------------------------------------------------------
// Type aliases
// ---------------------------------------------------------------------------

/** (cost, cityName) pair used in the priority queue. */
using PQEntry = std::pair<double, std::string>;

/** Min-heap: smallest cost at top. */
using MinPQ = std::priority_queue<PQEntry,
                                  std::vector<PQEntry>,
                                  std::greater<PQEntry>>;

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

/**
 * @brief Maps an OptimizationMode to the appropriate Road getter value.
 *        This is the SINGLE point of mode selection in the entire algorithm.
 */
double Dijkstra::getWeight(const Road& road, OptimizationMode mode) {
    switch (mode) {
        case OptimizationMode::Distance:   return road.getDistance();
        case OptimizationMode::TravelTime: return road.getTravelTime();
        case OptimizationMode::FuelCost:   return road.getFuelCost();
    }
    return road.getDistance(); // Unreachable; silences compiler warnings.
}

/**
 * @brief Walks the parent map backwards from @p dest to @p source
 *        and returns the city sequence in forward order.
 */
std::vector<std::string>
Dijkstra::reconstructPath(
        const std::unordered_map<std::string, std::string>& parent,
        const std::string& source,
        const std::string& dest) {

    std::vector<std::string> path;
    std::string current = dest;

    // Walk backwards until we hit the source (whose parent entry is "").
    while (!current.empty()) {
        path.push_back(current);
        auto it = parent.find(current);
        if (it == parent.end()) break;  // Shouldn't happen; defensive guard.
        current = it->second;
    }

    std::reverse(path.begin(), path.end());

    // Sanity check: the reconstructed path must start at source.
    if (path.empty() || path.front() != source) {
        return {}; // No valid path found.
    }
    return path;
}

/**
 * @brief Iterates the reconstructed path and sums all three edge weights
 *        by looking up each consecutive city pair in the adjacency list.
 */
void Dijkstra::computePathStats(const Graph&                    graph,
                                const std::vector<std::string>& path,
                                double& totalDistance,
                                double& totalTime,
                                double& totalFuel) {
    totalDistance = 0.0;
    totalTime     = 0.0;
    totalFuel     = 0.0;

    for (size_t i = 0; i + 1 < path.size(); ++i) {
        const std::string& from = path[i];
        const std::string& to   = path[i + 1];

        const City* city = graph.searchCity(from);
        if (!city) continue;

        for (const Road& road : city->getRoads()) {
            if (road.getDestination() == to) {
                totalDistance += road.getDistance();
                totalTime     += road.getTravelTime();
                totalFuel     += road.getFuelCost();
                break;
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Primary interface
// ---------------------------------------------------------------------------

RouteResult Dijkstra::findRoute(const Graph&       graph,
                                const std::string& sourceName,
                                const std::string& destName,
                                OptimizationMode   mode) {
    RouteResult result;
    result.mode = mode;

    // ------------------------------------------------------------------
    // Guard: empty graph
    // ------------------------------------------------------------------
    if (graph.getCityCount() == 0) {
        std::cerr << "[Dijkstra] Error: The graph is empty.\n";
        return result;
    }

    // ------------------------------------------------------------------
    // Guard: source city must exist
    // ------------------------------------------------------------------
    if (!graph.searchCity(sourceName)) {
        std::cerr << "[Dijkstra] Error: Source city not found: \""
                  << sourceName << "\"\n";
        return result;
    }

    // ------------------------------------------------------------------
    // Guard: destination city must exist
    // ------------------------------------------------------------------
    if (!graph.searchCity(destName)) {
        std::cerr << "[Dijkstra] Error: Destination city not found: \""
                  << destName << "\"\n";
        return result;
    }

    // ------------------------------------------------------------------
    // Trivial case: source == destination
    // ------------------------------------------------------------------
    if (sourceName == destName) {
        result.found         = true;
        result.path          = { sourceName };
        result.totalDistance = 0.0;
        result.totalTime     = 0.0;
        result.totalFuel     = 0.0;
        result.citiesVisited = 1;
        return result;
    }

    // ------------------------------------------------------------------
    // Initialise distance table and parent table
    // ------------------------------------------------------------------
    std::unordered_map<std::string, double> dist;
    std::unordered_map<std::string, std::string> parent;

    // Initialise all cities to infinity; source gets cost 0.
    for (const auto& pair : graph.getCities()) {
        dist[pair.first]   = INF;
        parent[pair.first] = "";  // Empty string = no predecessor.
    }
    dist[sourceName] = 0.0;

    // ------------------------------------------------------------------
    // Min-priority queue: (cost, city)
    // ------------------------------------------------------------------
    MinPQ pq;
    pq.push({ 0.0, sourceName });

    // Settled-city set: once a city is settled its shortest path is final.
    std::unordered_map<std::string, bool> settled;
    int settledCount = 0;

    // ------------------------------------------------------------------
    // Main Dijkstra loop
    // ------------------------------------------------------------------
    while (!pq.empty()) {
        // Extract the city with the smallest known cost.
        PQEntry top    = pq.top();
        double  currentCost = top.first;
        std::string currentName = top.second;
        pq.pop();

        // Skip stale entries (a city may appear multiple times in the PQ
        // with different costs due to lazy deletion).
        if (settled.count(currentName)) continue;
        settled[currentName] = true;
        ++settledCount;

        // Early exit: we have settled the destination.
        if (currentName == destName) break;

        // Retrieve the city's adjacency list.
        const City* currentCity = graph.searchCity(currentName);
        if (!currentCity) continue; // Defensive; shouldn't occur.

        // Relax each outgoing edge.
        for (const Road& road : currentCity->getRoads()) {
            const std::string& neighbourName = road.getDestination();

            // Skip already-settled neighbours.
            if (settled.count(neighbourName)) continue;

            double edgeWeight    = getWeight(road, mode);
            double newCost       = currentCost + edgeWeight;

            // Relaxation: update if a cheaper path is found.
            if (newCost < dist[neighbourName]) {
                dist[neighbourName]   = newCost;
                parent[neighbourName] = currentName;
                pq.push({ newCost, neighbourName });
            }
        }
    }

    // ------------------------------------------------------------------
    // Check if destination was reached
    // ------------------------------------------------------------------
    if (dist[destName] == INF) {
        std::cerr << "[Dijkstra] No route found from \""
                  << sourceName << "\" to \"" << destName << "\".\n";
        result.citiesVisited = settledCount;
        return result;
    }

    // ------------------------------------------------------------------
    // Reconstruct path and compute all-weight totals
    // ------------------------------------------------------------------
    result.path = reconstructPath(parent, sourceName, destName);

    if (result.path.empty()) {
        std::cerr << "[Dijkstra] Path reconstruction failed.\n";
        return result;
    }

    computePathStats(graph, result.path,
                     result.totalDistance,
                     result.totalTime,
                     result.totalFuel);

    result.found         = true;
    result.citiesVisited = settledCount;
    return result;
}

// ---------------------------------------------------------------------------
// Display helpers
// ---------------------------------------------------------------------------

std::string Dijkstra::modeToString(OptimizationMode mode) {
    switch (mode) {
        case OptimizationMode::Distance:   return "Shortest Distance";
        case OptimizationMode::TravelTime: return "Fastest Travel Time";
        case OptimizationMode::FuelCost:   return "Minimum Fuel Cost";
    }
    return "Unknown";
}

void Dijkstra::displayResult(const RouteResult& result) {
    const std::string thick(62, '=');
    const std::string thin(62, '-');

    std::cout << "\n" << thick << "\n"
              << "  ROUTE RESULT  |  Mode: " << modeToString(result.mode) << "\n"
              << thick << "\n";

    if (!result.found) {
        std::cout << "  [!] No route found.\n"
                  << "      Cities explored before giving up: "
                  << result.citiesVisited << "\n"
                  << thick << "\n\n";
        return;
    }

    // ------------------------------------------------------------------
    // Trivial same-source-destination case
    // ------------------------------------------------------------------
    if (result.path.size() == 1) {
        std::cout << "  Source and destination are the same city: "
                  << result.path.front() << "\n"
                  << "  No travel required.\n"
                  << thick << "\n\n";
        return;
    }

    // ------------------------------------------------------------------
    // Route banner
    // ------------------------------------------------------------------
    std::cout << "  Route  : ";
    for (size_t i = 0; i < result.path.size(); ++i) {
        if (i > 0) std::cout << "  ->  ";
        std::cout << result.path[i];
    }
    std::cout << "\n" << thin << "\n";

    // ------------------------------------------------------------------
    // Leg count
    // ------------------------------------------------------------------
    std::cout << "  Stops  : " << (result.path.size() - 1) << " leg(s)  |  "
              << result.path.size() << " cities\n"
              << thin << "\n";

    // ------------------------------------------------------------------
    // Summary statistics
    // ------------------------------------------------------------------
    std::cout << std::fixed;

    std::cout << "  Total Distance  : "
              << std::setprecision(1) << result.totalDistance << " km\n";

    std::cout << "  Total Time      : "
              << std::setprecision(1) << result.totalTime << " hours  ("
              << static_cast<int>(result.totalTime) << "h "
              << static_cast<int>((result.totalTime -
                                   static_cast<int>(result.totalTime)) * 60)
              << "m)\n";

    std::cout << "  Total Fuel Cost : PKR "
              << std::setprecision(0) << result.totalFuel << "\n";

    std::cout << thin << "\n";
    std::cout << "  Cities Explored by Dijkstra: "
              << result.citiesVisited << "\n";
    std::cout << thick << "\n\n";
}

// ---------------------------------------------------------------------------
// Input helpers
// ---------------------------------------------------------------------------

OptimizationMode Dijkstra::promptMode() {
    std::cout << "\n  Optimization Modes:\n"
              << "    1. Shortest Distance  (km)\n"
              << "    2. Fastest Travel Time (hours)\n"
              << "    3. Minimum Fuel Cost  (PKR)\n"
              << "  Enter choice [1-3]: ";

    int choice = 0;
    while (true) {
        std::cin >> choice;
        if (std::cin.fail() || choice < 1 || choice > 3) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "  Invalid input. Please enter 1, 2, or 3: ";
        }
        else {
            std::cin.ignore(10000, '\n');
            break;
        }
    }

    switch (choice) {
        case 1: return OptimizationMode::Distance;
        case 2: return OptimizationMode::TravelTime;
        case 3: return OptimizationMode::FuelCost;
        default: return OptimizationMode::Distance;
    }
}
