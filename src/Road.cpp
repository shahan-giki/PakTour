/**
 * @file Road.cpp
 * @brief Implements the Road class.
 *
 * PakTour Planner – Phase 1: Graph Foundation
 */

#include "../include/Road.h"

#include <iostream>
#include <iomanip>

// ---------------------------------------------------------------------------
// Constructors
// ---------------------------------------------------------------------------

Road::Road()
    : m_destination("")
    , m_distance(0.0)
    , m_travelTime(0.0)
    , m_fuelCost(0.0)
    , m_condition("")
{}

Road::Road(const std::string& destination,
           double             distance,
           double             travelTime,
           double             fuelCost,
           const std::string& condition)
    : m_destination(destination)
    , m_distance(distance)
    , m_travelTime(travelTime)
    , m_fuelCost(fuelCost)
    , m_condition(condition)
{}

// ---------------------------------------------------------------------------
// Getters
// ---------------------------------------------------------------------------

const std::string& Road::getDestination() const {
    return m_destination;
}

double Road::getDistance() const {
    return m_distance;
}

double Road::getTravelTime() const {
    return m_travelTime;
}

double Road::getFuelCost() const {
    return m_fuelCost;
}

const std::string& Road::getCondition() const {
    return m_condition;
}

// ---------------------------------------------------------------------------
// Display
// ---------------------------------------------------------------------------

void Road::displayInfo() const {
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "  -> " << m_destination << "\n"
              << "     Distance   : " << m_distance   << " km\n"
              << "     Time       : " << m_travelTime << " hours\n"
              << "     Fuel Cost  : " << std::fixed << std::setprecision(0)
              << m_fuelCost << " PKR\n"
              << "     Condition  : " << m_condition  << "\n";
}
