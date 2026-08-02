/**
 * @file RestaurantManager.cpp
 * @brief Implements the RestaurantManager class for PakTour Planner.
 */

#include "../include/RestaurantManager.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

std::string RestaurantManager::trim(const std::string& s) {
    const std::string ws = " \t\r\n";
    size_t start = s.find_first_not_of(ws);
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(ws);
    return s.substr(start, end - start + 1);
}

std::vector<std::string> RestaurantManager::parseCSVLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string token;
    bool inQuotes = false;

    for (size_t i = 0; i < line.length(); ++i) {
        char c = line[i];
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            fields.push_back(trim(token));
            token.clear();
        } else {
            token += c;
        }
    }
    fields.push_back(trim(token));
    return fields;
}

int RestaurantManager::loadRestaurants(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[Error] Cannot open restaurants CSV: " << filepath << "\n";
        return 0;
    }

    std::string line;

    // Skip header line
    if (!std::getline(file, line)) {
        std::cerr << "[Error] Restaurants CSV is empty: " << filepath << "\n";
        return 0;
    }

    int count = 0;
    int lineNum = 1;

    while (std::getline(file, line)) {
        ++lineNum;
        std::string cleanedLine = trim(line);
        if (cleanedLine.empty()) continue;

        std::vector<std::string> fields = parseCSVLine(cleanedLine);

        // Columns: city,name,cuisine,rating,image
        if (fields.size() < 5) {
            std::cerr << "[Warning] Invalid restaurant CSV line " << lineNum
                      << " in " << filepath << " – skipping.\n";
            continue;
        }

        try {
            std::string city    = fields[0];
            std::string name    = fields[1];
            std::string cuisine = fields[2];
            double rating       = std::stod(fields[3]);
            std::string image   = fields[4];

            if (city.empty() || name.empty()) continue;

            Restaurant restaurant{ name, cuisine, rating, image };
            m_restaurants[city].push_back(restaurant);
            ++count;
        } catch (const std::exception& e) {
            std::cerr << "[Warning] Parse error at line " << lineNum
                      << " in " << filepath << ": " << e.what() << "\n";
        }
    }

    std::cout << "[Info] Loaded " << count << " restaurants across "
              << m_restaurants.size() << " cities from " << filepath << "\n";
    return count;
}

bool RestaurantManager::hasCity(const std::string& cityName) const {
    auto it = m_restaurants.find(cityName);
    return (it != m_restaurants.end() && !it->second.empty());
}

std::vector<Restaurant> RestaurantManager::getRestaurants(const std::string& cityName) const {
    auto it = m_restaurants.find(cityName);
    if (it == m_restaurants.end()) return {};
    return it->second;
}

std::vector<Restaurant> RestaurantManager::getRestaurantsSortedByRating(const std::string& cityName) const {
    std::vector<Restaurant> list = getRestaurants(cityName);
    std::sort(list.begin(), list.end(), [](const Restaurant& a, const Restaurant& b) {
        return a.rating > b.rating; // Descending by rating
    });
    return list;
}

size_t RestaurantManager::getCityCount() const {
    return m_restaurants.size();
}

size_t RestaurantManager::getTotalRestaurantCount() const {
    size_t total = 0;
    for (const auto& pair : m_restaurants) {
        total += pair.second.size();
    }
    return total;
}
