/**
 * @file Dijkstra.h
 * @brief Declares the Dijkstra class and its supporting types for
 *        PakTour Planner – Phase 2.
 *
 * A single generic Dijkstra implementation supports three optimization
 * modes (Distance / Travel Time / Fuel Cost) via a runtime enum selector.
 * The algorithm operates directly on the Phase-1 Graph/City/Road classes
 * and does not modify them in any way.
 *
 * Complexity target: O((V + E) log V)
 *   - Priority queue  : std::priority_queue (binary min-heap)
 *   - Distance table  : std::unordered_map<string, double>
 *   - Parent table    : std::unordered_map<string, string>
 *   - Path            : std::vector<string>
 *
 * PakTour Planner – Phase 2: Dijkstra's Algorithm
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <limits>

#include "Graph.h"   // const Graph& is the sole input to the algorithm

// ---------------------------------------------------------------------------
// OptimizationMode
// ---------------------------------------------------------------------------

/**
 * @enum OptimizationMode
 * @brief Selects which Road weight Dijkstra minimises.
 *
 * The same algorithm runs for all three modes; the mode controls only
 * which getter is called when comparing edge weights.
 */
enum class OptimizationMode {
    Distance,    ///< Minimise total distance (km).
    TravelTime,  ///< Minimise total travel time (hours).
    FuelCost     ///< Minimise total fuel cost (PKR).
};

// ---------------------------------------------------------------------------
// RouteResult
// ---------------------------------------------------------------------------

/**
 * @struct RouteResult
 * @brief Aggregates the full output produced by one Dijkstra run.
 *
 * Always check @p found before reading the other fields.
 */
struct RouteResult {
    bool                     found;         ///< true iff a path exists.
    std::vector<std::string> path;          ///< Ordered list: source → ... → dest.
    double                   totalDistance; ///< Sum of Road::getDistance() along the path (km).
    double                   totalTime;     ///< Sum of Road::getTravelTime() along the path (hrs).
    double                   totalFuel;     ///< Sum of Road::getFuelCost()    along the path (PKR).
    int                      citiesVisited; ///< Number of cities *settled* by Dijkstra (including src/dst).
    OptimizationMode         mode;          ///< The mode used for this run.

    /**
     * @brief Default constructor – initialises to "not found".
     */
    RouteResult()
        : found(false)
        , totalDistance(0.0)
        , totalTime(0.0)
        , totalFuel(0.0)
        , citiesVisited(0)
        , mode(OptimizationMode::Distance)
    {}
};

// ---------------------------------------------------------------------------
// Dijkstra
// ---------------------------------------------------------------------------

/**
 * @class Dijkstra
 * @brief Provides static factory methods that run Dijkstra's algorithm
 *        on a Graph for a chosen optimization mode.
 *
 * The class is stateless – all methods are static.  It does not store
 * any graph data itself; it works exclusively through the public API of
 * the Graph, City, and Road classes from Phase 1.
 */
class Dijkstra {
public:
    // ------------------------------------------------------------------
    // Primary interface
    // ------------------------------------------------------------------

    /**
     * @brief Runs Dijkstra from @p sourceName to @p destName on @p graph,
     *        minimising the weight selected by @p mode.
     *
     * @param graph      Const reference to the Phase-1 graph.
     * @param sourceName Name of the start city.
     * @param destName   Name of the destination city.
     * @param mode       Which edge weight to minimise.
     * @return           RouteResult containing path, statistics, and a
     *                   @p found flag.
     *
     * Error conditions (RouteResult::found == false):
     *  - Source or destination city not in graph.
     *  - Source == destination (treated as trivial; path = {source},
     *    all totals = 0, found = true).
     *  - No connected path exists between the two cities.
     *  - Graph is empty.
     */
    static RouteResult findRoute(const Graph&       graph,
                                 const std::string& sourceName,
                                 const std::string& destName,
                                 OptimizationMode   mode);

    // ------------------------------------------------------------------
    // Display helpers
    // ------------------------------------------------------------------

    /**
     * @brief Pretty-prints a RouteResult to standard output.
     *        Includes route, per-leg breakdown, and summary statistics.
     * @param result The result to display.
     */
    static void displayResult(const RouteResult& result);

    /**
     * @brief Converts an OptimizationMode to a human-readable string.
     */
    static std::string modeToString(OptimizationMode mode);

    // ------------------------------------------------------------------
    // Input helpers (used by main.cpp)
    // ------------------------------------------------------------------

    /**
     * @brief Prompts the user to pick an OptimizationMode interactively.
     * @return The selected mode.
     */
    static OptimizationMode promptMode();

private:
    // ------------------------------------------------------------------
    // Internal helpers
    // ------------------------------------------------------------------

    /**
     * @brief Returns the edge weight of @p road for the given @p mode.
     *        This is the single point where mode selection happens.
     */
    static double getWeight(const Road& road, OptimizationMode mode);

    /**
     * @brief Reconstructs the ordered path from source to destination
     *        using the parent table filled during Dijkstra's execution.
     *
     * @param parent  Map: city → predecessor city name.
     * @param source  Start city name.
     * @param dest    End city name.
     * @return        Ordered vector from source to destination.
     */
    static std::vector<std::string>
    reconstructPath(const std::unordered_map<std::string, std::string>& parent,
                    const std::string& source,
                    const std::string& dest);

    /**
     * @brief Computes per-leg totals (distance / time / fuel) for a
     *        reconstructed path by walking the adjacency list.
     *
     * @param graph  The graph (for edge lookup).
     * @param path   The ordered city sequence.
     * @param[out] totalDistance  Filled with the sum of distances.
     * @param[out] totalTime      Filled with the sum of travel times.
     * @param[out] totalFuel      Filled with the sum of fuel costs.
     */
    static void computePathStats(const Graph&                    graph,
                                 const std::vector<std::string>& path,
                                 double& totalDistance,
                                 double& totalTime,
                                 double& totalFuel);

    /** Infinity sentinel for the distance table. */
    static constexpr double INF = std::numeric_limits<double>::infinity();
};
