/**
 * @file City.cpp
 * @brief Implements the City class.
 *
 * PakTour Planner – Phase 1: Graph Foundation
 */

#include "../include/City.h"
#include "../include/Road.h"

#include <iostream>
#include <iomanip>

// ---------------------------------------------------------------------------
// Constructors
// ---------------------------------------------------------------------------

City::City()
    : m_id(0)
    , m_name("")
    , m_province("")
    , m_latitude(0.0)
    , m_longitude(0.0)
{}

City::City(int               id,
           const std::string& name,
           const std::string& province,
           double             latitude,
           double             longitude)
    : m_id(id)
    , m_name(name)
    , m_province(province)
    , m_latitude(latitude)
    , m_longitude(longitude)
{}

// ---------------------------------------------------------------------------
// Getters
// ---------------------------------------------------------------------------

int City::getId() const {
    return m_id;
}

const std::string& City::getName() const {
    return m_name;
}

const std::string& City::getProvince() const {
    return m_province;
}

double City::getLatitude() const {
    return m_latitude;
}

double City::getLongitude() const {
    return m_longitude;
}

const std::vector<Road>& City::getRoads() const {
    return m_roads;
}

std::vector<Road>& City::getRoads() {
    return m_roads;
}

// ---------------------------------------------------------------------------
// Setters
// ---------------------------------------------------------------------------

void City::setId(int id) {
    m_id = id;
}

void City::setName(const std::string& name) {
    m_name = name;
}

void City::setProvince(const std::string& province) {
    m_province = province;
}

void City::setLatitude(double latitude) {
    m_latitude = latitude;
}

void City::setLongitude(double longitude) {
    m_longitude = longitude;
}

// ---------------------------------------------------------------------------
// Road management
// ---------------------------------------------------------------------------

void City::addRoad(const Road& road) {
    m_roads.push_back(road);
}

// ---------------------------------------------------------------------------
// Display
// ---------------------------------------------------------------------------

void City::displayInfo() const {
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "[City #" << m_id << "] "
              << m_name << "\n"
              << "  Province  : " << m_province << "\n"
              << "  Latitude  : " << m_latitude << "\n"
              << "  Longitude : " << m_longitude << "\n";
}
