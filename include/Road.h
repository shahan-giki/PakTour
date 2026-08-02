/**
 * @file Road.h
 * @brief Declares the Road class representing a weighted, directed edge
 *        in the PakTour graph.  Because the graph is *undirected*, the
 *        Graph class adds two Road objects (one per direction) for every
 *        logical connection.
 *
 * PakTour Planner – Phase 1: Graph Foundation
 */

#pragma once

#include <string>

// ---------------------------------------------------------------------------
// Road
// ---------------------------------------------------------------------------

/**
 * @class Road
 * @brief Represents one directed edge: source city → destination city.
 *
 * The Road stores:
 *  - The name of the destination city (string key into the graph).
 *  - Distance in kilometres.
 *  - Estimated travel time in hours.
 *  - Approximate fuel cost in Pakistani Rupees (PKR).
 *  - A textual road-condition label (e.g. "Good", "Fair", "Poor").
 */
class Road {
public:
    // ------------------------------------------------------------------
    // Constructors
    // ------------------------------------------------------------------

    /**
     * @brief Default constructor – creates an empty, uninitialised Road.
     */
    Road();

    /**
     * @brief Full constructor.
     * @param destination  Name of the destination city.
     * @param distance     Road length in km.
     * @param travelTime   Estimated travel time in hours.
     * @param fuelCost     Approximate fuel cost in PKR.
     * @param condition    Textual road-condition label.
     */
    Road(const std::string& destination,
         double             distance,
         double             travelTime,
         double             fuelCost,
         const std::string& condition);

    // ------------------------------------------------------------------
    // Getters
    // ------------------------------------------------------------------

    const std::string& getDestination() const;
    double             getDistance()    const;
    double             getTravelTime()  const;
    double             getFuelCost()    const;
    const std::string& getCondition()   const;

    // ------------------------------------------------------------------
    // Display
    // ------------------------------------------------------------------

    /**
     * @brief Prints detailed road information to standard output.
     *        Used by Graph::displayGraph() when listing each city's
     *        neighbours.
     */
    void displayInfo() const;

private:
    std::string m_destination; ///< Name of the city this road leads to.
    double      m_distance;    ///< Road length in km.
    double      m_travelTime;  ///< Estimated travel time in hours.
    double      m_fuelCost;    ///< Approximate fuel cost in PKR.
    std::string m_condition;   ///< Textual road-condition label.
};
