/**
 * @file RestaurantManager.h
 * @brief Declares the Restaurant struct and RestaurantManager class for PakTour Planner.
 *
 * Responsibilities:
 *  - Parse data/restaurants.csv
 *  - Store restaurants mapped by city using std::unordered_map
 *  - Retrieve restaurants for a city sorted by rating descending
 *  - Safe error handling for missing/malformed CSV files
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>

// ---------------------------------------------------------------------------
// Restaurant Struct
// ---------------------------------------------------------------------------

/**
 * @struct Restaurant
 * @brief Represents a restaurant recommendation in a city.
 */
struct Restaurant {
    std::string name;        ///< Restaurant name.
    std::string cuisine;     ///< Type of cuisine (e.g. Pakistani, BBQ, Seafood).
    double rating;           ///< User rating (e.g. 4.8).
    std::string imagePath;   ///< Path to restaurant image.
};

// ---------------------------------------------------------------------------
// RestaurantManager Class
// ---------------------------------------------------------------------------

/**
 * @class RestaurantManager
 * @brief Manages loading, storing, and querying restaurant recommendations.
 */
class RestaurantManager {
public:
    RestaurantManager() = default;

    /**
     * @brief Loads restaurants from CSV file.
     * @param filepath Path to restaurants.csv.
     * @return Number of restaurants successfully loaded.
     */
    int loadRestaurants(const std::string& filepath);

    /**
     * @brief Checks if a city has restaurant data.
     */
    bool hasCity(const std::string& cityName) const;

    /**
     * @brief Gets all restaurants for a city.
     */
    std::vector<Restaurant> getRestaurants(const std::string& cityName) const;

    /**
     * @brief Gets all restaurants for a city sorted by rating descending.
     */
    std::vector<Restaurant> getRestaurantsSortedByRating(const std::string& cityName) const;

    /**
     * @brief Returns total number of cities with restaurant data.
     */
    size_t getCityCount() const;

    /**
     * @brief Returns total count of restaurants loaded.
     */
    size_t getTotalRestaurantCount() const;

private:
    static std::string trim(const std::string& s);
    static std::vector<std::string> parseCSVLine(const std::string& line);

    /** Maps City Name -> Vector of Restaurants */
    std::unordered_map<std::string, std::vector<Restaurant>> m_restaurants;
};
