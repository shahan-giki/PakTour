/**
 * @file AttractionManager.cpp
 * @brief Implements the AttractionManager class and Detour Recommendation Engine.
 */

#include "../include/AttractionManager.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>

std::string AttractionManager::trim(const std::string& s) {
    const std::string ws = " \t\r\n";
    size_t start = s.find_first_not_of(ws);
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(ws);
    return s.substr(start, end - start + 1);
}

std::vector<std::string> AttractionManager::parseCSVLine(const std::string& line) {
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

int AttractionManager::loadAttractions(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[Error] Cannot open attractions CSV: " << filepath << "\n";
        return 0;
    }

    std::string line;

    // Skip header line
    if (!std::getline(file, line)) {
        std::cerr << "[Error] Attractions CSV is empty: " << filepath << "\n";
        return 0;
    }

    int count = 0;
    int lineNum = 1;

    while (std::getline(file, line)) {
        ++lineNum;
        std::string cleanedLine = trim(line);
        if (cleanedLine.empty()) continue;

        std::vector<std::string> fields = parseCSVLine(cleanedLine);

        // Expected format: 17 columns
        // city,attraction,category,description,image,rating,popularity,entryFee,
        // visitDuration,bestSeason,openingHours,latitude,longitude,detourDistance,
        // extraTime,extraFuelCost,importantNotes
        if (fields.size() < 17) {
            std::cerr << "[Warning] Invalid attraction CSV line " << lineNum
                      << " in " << filepath << " (found " << fields.size()
                      << " fields, expected 17) – skipping.\n";
            continue;
        }

        try {
            Attraction attr;
            std::string city    = fields[0];
            attr.name           = fields[1];
            attr.category       = fields[2];
            attr.description    = fields[3];
            attr.imagePath      = fields[4];
            attr.rating         = std::stod(fields[5]);
            attr.popularity     = std::stoi(fields[6]);
            attr.entryFee       = std::stod(fields[7]);
            attr.visitDuration  = std::stod(fields[8]);
            attr.bestSeason     = fields[9];
            attr.openingHours   = fields[10];
            attr.latitude       = std::stod(fields[11]);
            attr.longitude      = std::stod(fields[12]);
            attr.detourDistance = std::stod(fields[13]);
            attr.extraTime      = std::stoi(fields[14]);
            attr.extraFuelCost  = std::stod(fields[15]);
            attr.importantNotes = fields[16];

            if (city.empty() || attr.name.empty()) continue;

            m_attractions[city].push_back(attr);
            ++count;
        } catch (const std::exception& e) {
            std::cerr << "[Warning] Parse error at line " << lineNum
                      << " in " << filepath << ": " << e.what() << "\n";
        }
    }

    std::cout << "[Info] Loaded " << count << " attractions across "
              << m_attractions.size() << " cities from " << filepath << "\n";
    return count;
}

bool AttractionManager::hasCity(const std::string& cityName) const {
    auto it = m_attractions.find(cityName);
    return (it != m_attractions.end() && !it->second.empty());
}

std::vector<Attraction> AttractionManager::getAttractions(const std::string& cityName) const {
    auto it = m_attractions.find(cityName);
    if (it == m_attractions.end()) return {};
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

AttractionRecommendation
AttractionManager::evaluateAttraction(const Attraction& attr, const std::string& currentSeason) {
    // Trip Value Score Formula:
    // Base rating score: rating * 12.0 (max 60)
    // Popularity score: popularity * 0.4 (max 40)
    // Season bonus: +15 if currentSeason matches bestSeason or bestSeason == "All Year"
    // Detour distance penalty: detourDistance * 0.6
    // Extra time penalty: (extraTime / 60.0) * 8.0
    // Extra fuel penalty: extraFuelCost * 0.01

    double ratingScore     = attr.rating * 12.0;
    double popularityScore = attr.popularity * 0.4;

    double seasonBonus = 0.0;
    if (iequals(attr.bestSeason, "All Year") || iequals(attr.bestSeason, currentSeason)) {
        seasonBonus = 15.0;
    }

    double distancePenalty = attr.detourDistance * 0.6;
    double timePenalty     = (attr.extraTime / 60.0) * 8.0;
    double fuelPenalty     = attr.extraFuelCost * 0.01;

    double score = ratingScore + popularityScore + seasonBonus
                 - distancePenalty - timePenalty - fuelPenalty;

    if (score < 0.0) score = 0.0;
    if (score > 100.0) score = 100.0;

    RecommendationLevel level;
    if (score >= 80.0) {
        level = RecommendationLevel::HighlyRecommended;
    } else if (score >= 60.0) {
        level = RecommendationLevel::Recommended;
    } else if (score >= 40.0) {
        level = RecommendationLevel::Optional;
    } else {
        level = RecommendationLevel::NotRecommended;
    }

    AttractionRecommendation rec;
    rec.attraction     = attr;
    rec.tripValueScore = score;
    rec.level          = level;
    rec.levelString    = levelToString(level);
    return rec;
}

std::string AttractionManager::levelToString(RecommendationLevel level) {
    switch (level) {
        case RecommendationLevel::HighlyRecommended: return "Highly Recommended";
        case RecommendationLevel::Recommended:       return "Recommended";
        case RecommendationLevel::Optional:          return "Optional";
        case RecommendationLevel::NotRecommended:     return "Not Recommended";
    }
    return "Optional";
}

std::vector<AttractionRecommendation>
AttractionManager::getRecommendationsForCity(const std::string& cityName,
                                            const std::string& currentSeason) const {
    std::vector<Attraction> raw = getAttractions(cityName);
    std::vector<AttractionRecommendation> list;

    for (const auto& attr : raw) {
        list.push_back(evaluateAttraction(attr, currentSeason));
    }

    // Sort descending by Trip Value Score
    std::sort(list.begin(), list.end(), [](const AttractionRecommendation& a, const AttractionRecommendation& b) {
        return a.tripValueScore > b.tripValueScore;
    });

    return list;
}

size_t AttractionManager::getCityCount() const {
    return m_attractions.size();
}

size_t AttractionManager::getTotalAttractionCount() const {
    size_t total = 0;
    for (const auto& pair : m_attractions) {
        total += pair.second.size();
    }
    return total;
}
