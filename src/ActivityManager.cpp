/**
 * @file ActivityManager.cpp
 * @brief Implements the ActivityManager class for PakTour Planner.
 */

#include "../include/ActivityManager.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

std::string ActivityManager::trim(const std::string& s) {
    const std::string ws = " \t\r\n";
    size_t start = s.find_first_not_of(ws);
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(ws);
    return s.substr(start, end - start + 1);
}

std::vector<std::string> ActivityManager::parseCSVLine(const std::string& line) {
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

int ActivityManager::loadActivities(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[Error] Cannot open activities CSV: " << filepath << "\n";
        return 0;
    }

    std::string line;

    // Skip header line
    if (!std::getline(file, line)) {
        std::cerr << "[Error] Activities CSV is empty: " << filepath << "\n";
        return 0;
    }

    int count = 0;
    int lineNum = 1;

    while (std::getline(file, line)) {
        ++lineNum;
        std::string cleanedLine = trim(line);
        if (cleanedLine.empty()) continue;

        std::vector<std::string> fields = parseCSVLine(cleanedLine);

        // Columns: city,name,category,description,image
        if (fields.size() < 5) {
            std::cerr << "[Warning] Invalid activity CSV line " << lineNum
                      << " in " << filepath << " – skipping.\n";
            continue;
        }

        std::string city        = fields[0];
        std::string name        = fields[1];
        std::string category    = fields[2];
        std::string description = fields[3];
        std::string image       = fields[4];

        if (city.empty() || name.empty()) continue;

        Activity activity{ name, category, description, image };
        m_activities[city].push_back(activity);
        ++count;
    }

    std::cout << "[Info] Loaded " << count << " activities across "
              << m_activities.size() << " cities from " << filepath << "\n";
    return count;
}

bool ActivityManager::hasCity(const std::string& cityName) const {
    auto it = m_activities.find(cityName);
    return (it != m_activities.end() && !it->second.empty());
}

std::vector<Activity> ActivityManager::getActivities(const std::string& cityName) const {
    auto it = m_activities.find(cityName);
    if (it == m_activities.end()) return {};
    return it->second;
}

static bool iequals(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::tolower(static_cast<unsigned char>(a[i])) !=
            std::tolower(static_cast<unsigned char>(b[i]))) {
            return false;
        }
    }
    return true;
}

std::vector<Activity> ActivityManager::getActivitiesByCategory(const std::string& cityName, const std::string& category) const {
    std::vector<Activity> all = getActivities(cityName);
    if (category.empty()) return all;

    std::vector<Activity> filtered;
    for (const auto& act : all) {
        if (iequals(act.category, category)) {
            filtered.push_back(act);
        }
    }
    return filtered;
}

size_t ActivityManager::getCityCount() const {
    return m_activities.size();
}

size_t ActivityManager::getTotalActivityCount() const {
    size_t total = 0;
    for (const auto& pair : m_activities) {
        total += pair.second.size();
    }
    return total;
}
