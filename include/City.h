/**
 * @file City.h
 * @brief Declares the City class representing a node (vertex) in the
 *        PakTour graph. Each City stores geographic / administrative
 *        metadata and owns a list of outgoing Road objects.
 *
 * PakTour Planner – Phase 1: Graph Foundation
 */

#pragma once

#include <string>
#include <vector>

// Forward-declare Road to break the circular dependency
// (City.h <-> Road.h).  The full definition is included in City.cpp.
class Road;

// ---------------------------------------------------------------------------
// City
// ---------------------------------------------------------------------------

/**
 * @class City
 * @brief Represents a single city vertex in Pakistan's road network.
 *
 * Each City object holds:
 *  - A unique numeric ID (assigned from the CSV).
 *  - A human-readable name (used as the graph key).
 *  - Province, latitude, and longitude for geographic context.
 *  - A vector of Road objects describing outgoing connections.
 */
class City {
public:
    // ------------------------------------------------------------------
    // Constructors
    // ------------------------------------------------------------------

    /**
     * @brief Default constructor – creates an empty, invalid City.
     *        Useful when a City needs to be default-initialised before
     *        its real data is available (e.g. look-up return values).
     */
    City();

    /**
     * @brief Full constructor.
     * @param id        Unique numeric city identifier.
     * @param name      City name (must be unique across the graph).
     * @param province  Province / territory the city belongs to.
     * @param latitude  Geographic latitude  (decimal degrees).
     * @param longitude Geographic longitude (decimal degrees).
     */
    City(int id,
         const std::string& name,
         const std::string& province,
         double latitude,
         double longitude);

    // ------------------------------------------------------------------
    // Getters
    // ------------------------------------------------------------------

    int                       getId()        const;
    const std::string&        getName()      const;
    const std::string&        getProvince()  const;
    double                    getLatitude()  const;
    double                    getLongitude() const;

    /**
     * @brief Returns a read-only reference to the list of roads
     *        leaving this city.
     */
    const std::vector<Road>&  getRoads()     const;

    /**
     * @brief Returns a mutable reference so the Graph can append roads.
     */
    std::vector<Road>&        getRoads();

    // ------------------------------------------------------------------
    // Setters
    // ------------------------------------------------------------------

    void setId(int id);
    void setName(const std::string& name);
    void setProvince(const std::string& province);
    void setLatitude(double latitude);
    void setLongitude(double longitude);

    // ------------------------------------------------------------------
    // Road management
    // ------------------------------------------------------------------

    /**
     * @brief Appends a Road to this city's adjacency list.
     * @param road The Road to add.
     */
    void addRoad(const Road& road);

    // ------------------------------------------------------------------
    // Display
    // ------------------------------------------------------------------

    /**
     * @brief Prints basic city information (ID, name, province, coords)
     *        to standard output. Does NOT print connected roads.
     */
    void displayInfo() const;

private:
    int         m_id;
    std::string m_name;
    std::string m_province;
    double      m_latitude;
    double      m_longitude;

    /** Adjacency list: all roads going out from this city. */
    std::vector<Road> m_roads;
};
