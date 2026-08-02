/**
 * @file HotelManager.cpp
 * @brief Implements the HotelManager class for PakTour Planner.
 */

#include "../include/HotelManager.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

std::string HotelManager::trim(const std::string& s) {
    const std::string ws = " \t\r\n";
    size_t start = s.find_first_not_of(ws);
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(ws);
    return s.substr(start, end - start + 1);
}

std::vector<std::string> HotelManager::parseCSVLine(const std::string& line) {
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

int HotelManager::loadHotels(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[Error] Cannot open hotels CSV: " << filepath << "\n";
        return 0;
    }

    std::string line;

    // Skip header line
    if (!std::getline(file, line)) {
        std::cerr << "[Error] Hotels CSV is empty: " << filepath << "\n";
        return 0;
    }

    int count = 0;
    int lineNum = 1;

    while (std::getline(file, line)) {
        ++lineNum;
        std::string cleanedLine = trim(line);
        if (cleanedLine.empty()) continue;

        std::vector<std::string> fields = parseCSVLine(cleanedLine);

        // Columns: city,name,stars,pricePerNight,rating,image
        if (fields.size() < 6) {
            std::cerr << "[Warning] Invalid hotel CSV line " << lineNum
                      << " in " << filepath << " – skipping.\n";
            continue;
        }

        try {
            std::string city  = fields[0];
            std::string name  = fields[1];
            int stars         = std::stoi(fields[2]);
            double price      = std::stod(fields[3]);
            double rating     = std::stod(fields[4]);
            std::string image = fields[5];

            if (city.empty() || name.empty()) continue;

            Hotel hotel{ name, stars, price, rating, image };
            m_hotels[city].push_back(hotel);
            ++count;
        } catch (const std::exception& e) {
            std::cerr << "[Warning] Parse error at line " << lineNum
                      << " in " << filepath << ": " << e.what() << "\n";
        }
    }

    std::cout << "[Info] Loaded " << count << " hotels across "
              << m_hotels.size() << " cities from " << filepath << "\n";
    return count;
}

bool HotelManager::hasCity(const std::string& cityName) const {
    auto it = m_hotels.find(cityName);
    return (it != m_hotels.end() && !it->second.empty());
}

std::vector<Hotel> HotelManager::getHotels(const std::string& cityName) const {
    auto it = m_hotels.find(cityName);
    if (it == m_hotels.end()) return {};
    return it->second;
}

std::vector<Hotel> HotelManager::getHotelsSortedByRating(const std::string& cityName) const {
    std::vector<Hotel> list = getHotels(cityName);
    std::sort(list.begin(), list.end(), [](const Hotel& a, const Hotel& b) {
        return a.rating > b.rating; // Descending by rating
    });
    return list;
}

size_t HotelManager::getCityCount() const {
    return m_hotels.size();
}

size_t HotelManager::getTotalHotelCount() const {
    size_t total = 0;
    for (const auto& pair : m_hotels) {
        total += pair.second.size();
    }
    return total;
}
