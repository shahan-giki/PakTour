/**
 * @file HotelManager.h
 * @brief Declares the Hotel struct and HotelManager class for PakTour Planner.
 *
 * Responsibilities:
 *  - Parse data/hotels.csv
 *  - Store hotels mapped by city using std::unordered_map
 *  - Retrieve hotels for a city sorted by rating descending
 *  - Safe error handling for missing/malformed CSV files
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>

// ---------------------------------------------------------------------------
// Hotel Struct
// ---------------------------------------------------------------------------

/**
 * @struct Hotel
 * @brief Represents a hotel recommendation in a city.
 */
struct Hotel {
    std::string name;          ///< Hotel name.
    int stars;                 ///< Star rating (1-5).
    double pricePerNight;      ///< Price per night in PKR.
    double rating;             ///< User rating (e.g. 4.8).
    std::string imagePath;     ///< Path to hotel image.
};

// ---------------------------------------------------------------------------
// HotelManager Class
// ---------------------------------------------------------------------------

/**
 * @class HotelManager
 * @brief Manages loading, storing, and querying hotel recommendations.
 */
class HotelManager {
public:
    HotelManager() = default;

    /**
     * @brief Loads hotels from CSV file.
     * @param filepath Path to hotels.csv.
     * @return Number of hotels successfully loaded.
     */
    int loadHotels(const std::string& filepath);

    /**
     * @brief Checks if a city has hotel data.
     */
    bool hasCity(const std::string& cityName) const;

    /**
     * @brief Gets all hotels for a city.
     */
    std::vector<Hotel> getHotels(const std::string& cityName) const;

    /**
     * @brief Gets all hotels for a city sorted by rating descending.
     */
    std::vector<Hotel> getHotelsSortedByRating(const std::string& cityName) const;

    /**
     * @brief Returns total number of cities with hotel data.
     */
    size_t getCityCount() const;

    /**
     * @brief Returns total count of hotels loaded.
     */
    size_t getTotalHotelCount() const;

private:
    static std::string trim(const std::string& s);
    static std::vector<std::string> parseCSVLine(const std::string& line);

    /** Maps City Name -> Vector of Hotels */
    std::unordered_map<std::string, std::vector<Hotel>> m_hotels;
};
