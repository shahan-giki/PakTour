/**
 * @file Graph.cpp
 * @brief Implements the Graph class: adjacency-list management, CSV
 *        parsing, duplicate prevention, and graph display.
 *
 * PakTour Planner – Phase 1: Graph Foundation
 */

#include "../include/Graph.h"
#include "../include/City.h"
#include "../include/Road.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iomanip>

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

/**
 * @brief Removes leading and trailing whitespace (spaces, tabs, carriage
 *        returns) from @p s.  Used to clean up CSV fields.
 */
std::string Graph::trim(const std::string& s) {
    const std::string ws = " \t\r\n";
    size_t start = s.find_first_not_of(ws);
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(ws);
    return s.substr(start, end - start + 1);
}

/**
 * @brief Checks whether a directed edge from @p source to @p dest
 *        already exists in source's Road list.
 */
bool Graph::roadExists(const City& source, const std::string& dest) const {
    for (const Road& road : source.getRoads()) {
        if (road.getDestination() == dest) {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// City Operations
// ---------------------------------------------------------------------------

bool Graph::addCity(const City& city) {
    // Prevent duplicate city names.
    if (m_cities.count(city.getName())) {
        std::cerr << "[Warning] Duplicate city ignored: \""
                  << city.getName() << "\"\n";
        return false;
    }
    m_cities.emplace(city.getName(), city);
    return true;
}

City* Graph::searchCity(const std::string& name) {
    auto it = m_cities.find(name);
    if (it == m_cities.end()) return nullptr;
    return &it->second;
}

const City* Graph::searchCity(const std::string& name) const {
    auto it = m_cities.find(name);
    if (it == m_cities.end()) return nullptr;
    return &it->second;
}

// ---------------------------------------------------------------------------
// Edge Operations
// ---------------------------------------------------------------------------

bool Graph::connectCities(const std::string& sourceName,
                          const std::string& destName,
                          double             distance,
                          double             travelTime,
                          double             fuelCost,
                          const std::string& condition) {
    // Both cities must already exist.
    City* src = searchCity(sourceName);
    City* dst = searchCity(destName);

    if (!src) {
        std::cerr << "[Warning] Cannot connect – source city not found: \""
                  << sourceName << "\"\n";
        return false;
    }
    if (!dst) {
        std::cerr << "[Warning] Cannot connect – destination city not found: \""
                  << destName << "\"\n";
        return false;
    }

    // Prevent duplicate forward edge.
    if (roadExists(*src, destName)) {
        std::cerr << "[Warning] Duplicate road ignored: \""
                  << sourceName << "\" -> \"" << destName << "\"\n";
        return false;
    }

    // Add the forward edge: source -> destination.
    src->addRoad(Road(destName, distance, travelTime, fuelCost, condition));

    // Add the reverse edge: destination -> source  (undirected graph).
    if (!roadExists(*dst, sourceName)) {
        dst->addRoad(Road(sourceName, distance, travelTime, fuelCost, condition));
    }

    return true;
}

// ---------------------------------------------------------------------------
// CSV Loaders
// ---------------------------------------------------------------------------

int Graph::loadCitiesFromCSV(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[Error] Cannot open cities CSV: " << filepath << "\n";
        return 0;
    }

    std::string line;

    // Skip the header row.
    if (!std::getline(file, line)) {
        std::cerr << "[Error] Cities CSV is empty: " << filepath << "\n";
        return 0;
    }

    int count = 0;
    int lineNum = 1;

    while (std::getline(file, line)) {
        ++lineNum;
        line = trim(line);
        if (line.empty()) continue;   // Skip blank lines.

        std::istringstream ss(line);
        std::string token;
        std::vector<std::string> fields;

        while (std::getline(ss, token, ',')) {
            fields.push_back(trim(token));
        }

        // Expected columns: CityID, CityName, Province, Latitude, Longitude
        if (fields.size() < 5) {
            std::cerr << "[Warning] Invalid CSV format at line "
                      << lineNum << " in " << filepath
                      << " – skipping.\n";
            continue;
        }

        try {
            int    id        = std::stoi(fields[0]);
            std::string name = fields[1];
            std::string prov = fields[2];
            double lat       = std::stod(fields[3]);
            double lon       = std::stod(fields[4]);

            if (name.empty()) {
                std::cerr << "[Warning] Empty city name at line "
                          << lineNum << " – skipping.\n";
                continue;
            }

            City city(id, name, prov, lat, lon);
            if (addCity(city)) ++count;
        }
        catch (const std::exception& e) {
            std::cerr << "[Warning] Parse error at line " << lineNum
                      << " in " << filepath << ": " << e.what()
                      << " – skipping.\n";
        }
    }

    std::cout << "[Info] Loaded " << count << " cities from " << filepath << "\n";
    return count;
}

int Graph::loadRoadsFromCSV(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[Error] Cannot open roads CSV: " << filepath << "\n";
        return 0;
    }

    std::string line;

    // Skip the header row.
    if (!std::getline(file, line)) {
        std::cerr << "[Error] Roads CSV is empty: " << filepath << "\n";
        return 0;
    }

    int count = 0;
    int lineNum = 1;

    while (std::getline(file, line)) {
        ++lineNum;
        line = trim(line);
        if (line.empty()) continue;

        std::istringstream ss(line);
        std::string token;
        std::vector<std::string> fields;

        while (std::getline(ss, token, ',')) {
            fields.push_back(trim(token));
        }

        // Expected: SourceCity, DestinationCity, Distance, TravelTime,
        //           FuelCost, RoadCondition
        if (fields.size() < 6) {
            std::cerr << "[Warning] Invalid CSV format at line "
                      << lineNum << " in " << filepath
                      << " – skipping.\n";
            continue;
        }

        try {
            std::string src       = fields[0];
            std::string dst       = fields[1];
            double distance       = std::stod(fields[2]);
            double travelTime     = std::stod(fields[3]);
            double fuelCost       = std::stod(fields[4]);
            std::string condition = fields[5];

            if (src.empty() || dst.empty()) {
                std::cerr << "[Warning] Empty city name at line "
                          << lineNum << " – skipping.\n";
                continue;
            }

            if (connectCities(src, dst, distance, travelTime,
                               fuelCost, condition)) {
                ++count;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "[Warning] Parse error at line " << lineNum
                      << " in " << filepath << ": " << e.what()
                      << " – skipping.\n";
        }
    }

    std::cout << "[Info] Loaded " << count << " roads from " << filepath << "\n";
    return count;
}

// ---------------------------------------------------------------------------
// Display
// ---------------------------------------------------------------------------

void Graph::displayGraph() const {
    const std::string separator(40, '-');

    std::cout << "\n"
              << "============================================================\n"
              << "           PAKTOUR PLANNER – ROAD NETWORK GRAPH             \n"
              << "============================================================\n"
              << "  Total Cities : " << getCityCount() << "\n"
              << "  Total Roads  : " << (getEdgeCount() / 2) << "\n"
              << "============================================================\n\n";

    for (const auto& pair : m_cities) {
        const City& city = pair.second;
        std::cout << city.getName() << "\n";

        const std::vector<Road>& roads = city.getRoads();

        if (roads.empty()) {
            std::cout << "  (no connections)\n";
        }
        else {
            for (const Road& road : roads) {
                road.displayInfo();
            }
        }

        std::cout << separator << "\n\n";
    }
}

// ---------------------------------------------------------------------------
// Utility
// ---------------------------------------------------------------------------

int Graph::getCityCount() const {
    return static_cast<int>(m_cities.size());
}

int Graph::getEdgeCount() const {
    int total = 0;
    for (const auto& pair : m_cities) {
        total += static_cast<int>(pair.second.getRoads().size());
    }
    return total;
}

const std::unordered_map<std::string, City>& Graph::getCities() const {
    return m_cities;
}
