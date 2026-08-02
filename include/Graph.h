/**
 * @file Graph.h
 * @brief Declares the Graph class that models Pakistan's road network as
 *        a weighted, undirected adjacency-list graph.
 *
 * Internally the graph is stored as:
 *   std::unordered_map<std::string, City>
 * where the key is the city name and the value is the City object that
 * owns its outgoing Road list.
 *
 * PakTour Planner – Phase 1: Graph Foundation
 */

#pragma once

#include <string>
#include <unordered_map>

#include "City.h"   // City owns the adjacency list (vector<Road>)
#include "Road.h"

// ---------------------------------------------------------------------------
// Graph
// ---------------------------------------------------------------------------

/**
 * @class Graph
 * @brief Represents Pakistan's transportation network as a weighted,
 *        undirected adjacency-list graph.
 *
 * Responsibilities:
 *  - Store cities in an unordered_map keyed by city name.
 *  - Add cities while preventing duplicates.
 *  - Search for a city by name.
 *  - Connect two cities with an undirected edge (adds two Road objects).
 *  - Prevent duplicate road connections.
 *  - Load cities and roads from CSV files.
 *  - Display the complete adjacency list.
 */
class Graph {
public:
    // ------------------------------------------------------------------
    // Constructor
    // ------------------------------------------------------------------

    Graph() = default;

    // ------------------------------------------------------------------
    // City Operations
    // ------------------------------------------------------------------

    /**
     * @brief Inserts a City into the graph.
     * @param city The City object to insert.
     * @return true  if the city was added successfully.
     * @return false if a city with the same name already exists.
     */
    bool addCity(const City& city);

    /**
     * @brief Looks up a city by name.
     * @param name City name to search for.
     * @return Pointer to the City if found; nullptr otherwise.
     */
    City* searchCity(const std::string& name);

    /**
     * @brief Const overload of searchCity.
     */
    const City* searchCity(const std::string& name) const;

    // ------------------------------------------------------------------
    // Edge Operations
    // ------------------------------------------------------------------

    /**
     * @brief Adds an undirected road between two cities.
     *
     * Both cities must already exist in the graph.  The method creates
     * two directed Road objects (one per direction) so the graph remains
     * logically undirected.  Duplicate connections are silently ignored.
     *
     * @param sourceName      Name of the source city.
     * @param destName        Name of the destination city.
     * @param distance        Road length in km.
     * @param travelTime      Travel time in hours.
     * @param fuelCost        Fuel cost in PKR.
     * @param condition       Road condition label.
     * @return true  if the road was added.
     * @return false if either city is missing or the road already exists.
     */
    bool connectCities(const std::string& sourceName,
                       const std::string& destName,
                       double             distance,
                       double             travelTime,
                       double             fuelCost,
                       const std::string& condition);

    // ------------------------------------------------------------------
    // CSV Loaders
    // ------------------------------------------------------------------

    /**
     * @brief Parses cities.csv and inserts every city into the graph.
     * @param filepath Path to the CSV file.
     * @return Number of cities successfully loaded.
     */
    int loadCitiesFromCSV(const std::string& filepath);

    /**
     * @brief Parses roads.csv and connects cities in the graph.
     * @param filepath Path to the CSV file.
     * @return Number of roads successfully loaded.
     */
    int loadRoadsFromCSV(const std::string& filepath);

    // ------------------------------------------------------------------
    // Display
    // ------------------------------------------------------------------

    /**
     * @brief Prints the complete adjacency list to standard output.
     *
     * For each city, lists every neighbouring city together with the
     * road's distance, travel time, and fuel cost.
     */
    void displayGraph() const;

    // ------------------------------------------------------------------
    // Utility
    // ------------------------------------------------------------------

    /** @return Total number of cities (vertices) in the graph. */
    int getCityCount() const;

    /** @return Total number of directed edges stored (each road × 2). */
    int getEdgeCount() const;

    /**
     * @brief Returns a const reference to the internal city map.
     *        Intended for use by future algorithm modules (e.g. Dijkstra).
     */
    const std::unordered_map<std::string, City>& getCities() const;

private:
    // ------------------------------------------------------------------
    // Internal helpers
    // ------------------------------------------------------------------

    /**
     * @brief Checks whether a road from @p source to @p dest already
     *        exists in @p source 's adjacency list.
     */
    bool roadExists(const City& source, const std::string& dest) const;

    /**
     * @brief Trims leading and trailing whitespace from a string.
     *        Used when parsing CSV fields.
     */
    static std::string trim(const std::string& s);

    // ------------------------------------------------------------------
    // Data
    // ------------------------------------------------------------------

    /**
     * @brief The core adjacency-list structure.
     *        Key   = city name (unique).
     *        Value = City object owning its outgoing Road list.
     */
    std::unordered_map<std::string, City> m_cities;
};
