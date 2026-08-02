/**
 * @file AttractionManager.h
 * @brief Declares the Attraction struct, RecommendationLevel enum, and
 *        AttractionManager class for PakTour Planner – Detour Recommendation Engine.
 *
 * Responsibilities:
 *  - Store full metadata for tourist attractions
 *  - Calculate Trip Value Score considering ratings, popularity, detours, and season
 *  - Classify attractions as Highly Recommended, Recommended, Optional, or Not Recommended
 *  - Provide queries for city-level recommendations
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>

// ---------------------------------------------------------------------------
// Enums & Structs
// ---------------------------------------------------------------------------

/**
 * @enum RecommendationLevel
 * @brief Classification level based on calculated Trip Value Score.
 */
enum class RecommendationLevel {
    HighlyRecommended, ///< Outstanding value, minimal detour, matches season.
    Recommended,       ///< Good value, reasonable detour.
    Optional,          ///< Moderate value or higher detour cost.
    NotRecommended     ///< High detour cost relative to rating/season.
};

/**
 * @struct Attraction
 * @brief Represents a single tourist attraction with detour & visitor metadata.
 */
struct Attraction {
    std::string name;             ///< Attraction name.
    std::string category;         ///< Historical, Nature, Culture, Family, Adventure, etc.
    std::string description;      ///< Detailed description.
    std::string imagePath;        ///< Image artifact path.
    double rating;                ///< Rating out of 5.0.
    int popularity;               ///< Popularity score (1-100).
    double entryFee;              ///< Entry fee in PKR.
    double visitDuration;         ///< Recommended visit duration in hours.
    std::string bestSeason;       ///< Best travel season (Spring, Summer, Autumn, Winter, All Year).
    std::string openingHours;     ///< Opening hours string.
    double latitude;              ///< Latitude coordinates.
    double longitude;             ///< Longitude coordinates.
    double detourDistance;        ///< Additional distance from city in km.
    int extraTime;                ///< Additional driving time in minutes.
    double extraFuelCost;         ///< Additional fuel cost in PKR.
    std::string importantNotes;   ///< Visitor notes and instructions.
};

/**
 * @struct AttractionRecommendation
 * @brief Bundles an Attraction with its calculated Trip Value Score and Level.
 */
struct AttractionRecommendation {
    Attraction attraction;
    double tripValueScore;
    RecommendationLevel level;
    std::string levelString;
};

// ---------------------------------------------------------------------------
// AttractionManager Class
// ---------------------------------------------------------------------------

/**
 * @class AttractionManager
 * @brief Manages loading, scoring, and recommendation of tourist attractions.
 */
class AttractionManager {
public:
    AttractionManager() = default;

    /**
     * @brief Parses attractions.csv and populates the database.
     * @param filepath Path to attractions.csv.
     * @return Number of attractions successfully loaded.
     */
    int loadAttractions(const std::string& filepath);

    /**
     * @brief Checks if a city has registered attractions.
     */
    bool hasCity(const std::string& cityName) const;

    /**
     * @brief Gets all raw attractions stored for a city.
     */
    std::vector<Attraction> getAttractions(const std::string& cityName) const;

    /**
     * @brief Evaluates and ranks all attractions for a city for a given travel season.
     * @param cityName Name of the city.
     * @param currentSeason Travel season (Spring, Summer, Autumn, Winter).
     * @return Ranked list of AttractionRecommendation structs.
     */
    std::vector<AttractionRecommendation>
    getRecommendationsForCity(const std::string& cityName,
                              const std::string& currentSeason) const;

    /**
     * @brief Calculates the Trip Value Score and level for a single attraction.
     */
    static AttractionRecommendation
    evaluateAttraction(const Attraction& attr, const std::string& currentSeason);

    /**
     * @brief Converts RecommendationLevel enum to a display string.
     */
    static std::string levelToString(RecommendationLevel level);

    /**
     * @brief Total unique cities with attraction data.
     */
    size_t getCityCount() const;

    /**
     * @brief Total count of attractions loaded.
     */
    size_t getTotalAttractionCount() const;

private:
    static std::string trim(const std::string& s);
    static std::vector<std::string> parseCSVLine(const std::string& line);

    /** Maps City Name -> Vector of Attractions */
    std::unordered_map<std::string, std::vector<Attraction>> m_attractions;
};
