/**
 * @file ActivityManager.h
 * @brief Declares the Activity struct and ActivityManager class for PakTour Planner.
 *
 * Responsibilities:
 *  - Parse data/activities.csv
 *  - Store activities mapped by city using std::unordered_map
 *  - Filter activities for a city based on trip type preference (Historical, Food, Nature, Adventure, Family)
 *  - Safe error handling for missing/malformed CSV files
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>

// ---------------------------------------------------------------------------
// Activity Struct
// ---------------------------------------------------------------------------

/**
 * @struct Activity
 * @brief Represents an activity recommendation in a city.
 */
struct Activity {
    std::string name;          ///< Activity name.
    std::string category;      ///< Category (e.g., Historical, Food, Nature, Adventure, Family).
    std::string description;   ///< Activity description.
    std::string imagePath;     ///< Path to activity image.
};

// ---------------------------------------------------------------------------
// ActivityManager Class
// ---------------------------------------------------------------------------

/**
 * @class ActivityManager
 * @brief Manages loading, storing, and filtering activity recommendations.
 */
class ActivityManager {
public:
    ActivityManager() = default;

    /**
     * @brief Loads activities from CSV file.
     * @param filepath Path to activities.csv.
     * @return Number of activities successfully loaded.
     */
    int loadActivities(const std::string& filepath);

    /**
     * @brief Checks if a city has activity data.
     */
    bool hasCity(const std::string& cityName) const;

    /**
     * @brief Gets all activities for a city.
     */
    std::vector<Activity> getActivities(const std::string& cityName) const;

    /**
     * @brief Gets activities for a city filtered by category/trip preference.
     * @param cityName Name of the city.
     * @param category Filter category (e.g. "Historical", "Food", "Nature", "Adventure", "Family").
     * @return Vector of activities matching the category.
     */
    std::vector<Activity> getActivitiesByCategory(const std::string& cityName, const std::string& category) const;

    /**
     * @brief Returns total number of cities with activity data.
     */
    size_t getCityCount() const;

    /**
     * @brief Returns total count of activities loaded.
     */
    size_t getTotalActivityCount() const;

private:
    static std::string trim(const std::string& s);
    static std::vector<std::string> parseCSVLine(const std::string& line);

    /** Maps City Name -> Vector of Activities */
    std::unordered_map<std::string, std::vector<Activity>> m_activities;
};
