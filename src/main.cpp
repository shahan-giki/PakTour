/**
 * @file main.cpp
 * @brief Entry point for PakTour Planner – Smart Detour Recommendation Engine & Custom Itinerary Builder.
 *
 * Workflow:
 *   1. Load cities from data/cities.csv.
 *   2. Load roads from data/roads.csv.
 *   3. Load attractions with detour metadata from data/attractions.csv.
 *   4. Interactively prompt user for:
 *        a. Source city
 *        b. Destination city
 *        c. Optimization mode (Distance / Time / Fuel)
 *        d. Current travel season (Spring / Summer / Autumn / Winter)
 *   5. Run Dijkstra's algorithm and display shortest route details.
 *   6. For each city on the route:
 *        - Calculate & display Detour Trip Value Score and Recommendation Level
 *          (Highly Recommended, Recommended, Optional, Not Recommended).
 *        - Prompt user to ADD or SKIP each attraction to build a custom itinerary.
 *   7. Display the updated CUSTOM TRAVEL ITINERARY with recalculated total distance,
 *      total time (travel + detours + visit duration), and total financial cost.
 *   8. Offer option to plan another route or exit.
 *
 * PakTour Planner – Detour Recommendation Engine Upgrade
 */

#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include <algorithm>
#include <vector>

#include "../include/Graph.h"
#include "../include/City.h"
#include "../include/Dijkstra.h"
#include "../include/AttractionManager.h"

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static void printBanner() {
    std::cout
        << "\n"
        << "  ╔══════════════════════════════════════════════════════════╗\n"
        << "  ║          PakTour Planner  –  Tour & Detour Guide         ║\n"
        << "  ║         Smart Detour Recommendation Engine             ║\n"
        << "  ╚══════════════════════════════════════════════════════════╝\n\n";
}

static void printSectionLine(const std::string& title) {
    std::cout << "\n──────────────────────────────────────────────────────────\n"
              << "  " << title << "\n"
              << "──────────────────────────────────────────────────────────\n";
}

/**
 * @brief Reads a non-empty trimmed line from stdin with a custom prompt.
 */
static std::string promptCity(const std::string& label) {
    std::string input;
    while (true) {
        std::cout << "  " << label << ": ";
        std::getline(std::cin, input);

        size_t start = input.find_first_not_of(" \t\r\n");
        size_t end   = input.find_last_not_of(" \t\r\n");
        if (start != std::string::npos) {
            input = input.substr(start, end - start + 1);
        } else {
            input.clear();
        }

        if (!input.empty()) return input;
        std::cout << "  [!] City name cannot be empty. Please try again.\n";
    }
}

/**
 * @brief Prompts user to select the current travel season.
 */
static std::string promptTravelSeason() {
    std::cout << "\n  Select Current Travel Season:\n"
              << "    1. Spring\n"
              << "    2. Summer\n"
              << "    3. Autumn\n"
              << "    4. Winter\n"
              << "  Enter choice [1-4]: ";

    int choice = 0;
    while (true) {
        std::cin >> choice;
        if (std::cin.fail() || choice < 1 || choice > 4) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "  Invalid input. Please enter a number between 1 and 4: ";
        } else {
            std::cin.ignore(10000, '\n');
            break;
        }
    }

    switch (choice) {
        case 1: return "Spring";
        case 2: return "Summer";
        case 3: return "Autumn";
        case 4: return "Winter";
        default: return "Spring";
    }
}

/**
 * @brief Asks the user whether to run another search.
 */
static bool promptAnotherSearch() {
    std::cout << "\n  Would you like to plan another route? [y/n]: ";
    std::string ans;
    std::getline(std::cin, ans);
    return (!ans.empty() && (ans[0] == 'y' || ans[0] == 'Y'));
}

/**
 * @brief Asks a yes/no question to add an attraction.
 */
static bool promptAddAttraction(const std::string& attrName) {
    std::cout << "  Add \"" << attrName << "\" to your itinerary? [y/n]: ";
    std::string ans;
    std::getline(std::cin, ans);
    return (!ans.empty() && (ans[0] == 'y' || ans[0] == 'Y'));
}

// ---------------------------------------------------------------------------
// Struct for User Selected Itinerary Items
// ---------------------------------------------------------------------------

struct SelectedItineraryItem {
    std::string cityName;
    AttractionRecommendation recommendation;
};

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main() {
    printBanner();

    // ------------------------------------------------------------------
    // Load Datasets
    // ------------------------------------------------------------------
    printSectionLine("Loading Road Network & Attraction Datasets");

    Graph graph;

    int citiesLoaded = graph.loadCitiesFromCSV("data/cities.csv");
    if (citiesLoaded == 0) {
        std::cerr << "\n  [Fatal] No cities loaded. "
                     "Verify data/cities.csv exists and is valid.\n";
        return 1;
    }

    int roadsLoaded = graph.loadRoadsFromCSV("data/roads.csv");

    AttractionManager attractionManager;
    int attractionsLoaded = attractionManager.loadAttractions("data/attractions.csv");

    std::cout << "\n  Dataset summary:\n"
              << "    Cities      : " << graph.getCityCount() << "\n"
              << "    Roads       : " << roadsLoaded << " (logical), "
              << graph.getEdgeCount() << " directed edges\n"
              << "    Attractions : " << attractionsLoaded << " across "
              << attractionManager.getCityCount() << " cities\n";

    // ------------------------------------------------------------------
    // Available Cities
    // ------------------------------------------------------------------
    printSectionLine("Available Cities");
    std::vector<std::string> cityNames;
    for (const auto& pair : graph.getCities()) {
        cityNames.push_back(pair.first);
    }
    std::sort(cityNames.begin(), cityNames.end());

    int col = 0;
    for (const std::string& name : cityNames) {
        std::cout << "  " << std::left << std::setw(25) << name;
        if (++col % 3 == 0) std::cout << "\n";
    }
    if (col % 3 != 0) std::cout << "\n";

    // ------------------------------------------------------------------
    // Interactive Loop
    // ------------------------------------------------------------------
    bool continueLoop = true;
    while (continueLoop) {
        printSectionLine("Smart Route & Detour Planner");

        // (a) Source city
        std::string source = promptCity("Enter source city");
        if (!graph.searchCity(source)) {
            std::cout << "  [!] \"" << source
                      << "\" is not in the graph. Please check the city list above.\n";
            continue;
        }

        // (b) Destination city
        std::string destination = promptCity("Enter destination city");
        if (!graph.searchCity(destination)) {
            std::cout << "  [!] \"" << destination
                      << "\" is not in the graph. Please check the city list above.\n";
            continue;
        }

        // (c) Optimization mode
        OptimizationMode mode = Dijkstra::promptMode();

        // (d) Select current travel season
        std::string currentSeason = promptTravelSeason();

        // (e) Run Dijkstra
        std::cout << "\n  Calculating optimal route...\n";
        RouteResult baseResult = Dijkstra::findRoute(graph, source, destination, mode);

        // (f) Display base route details
        Dijkstra::displayResult(baseResult);

        if (!baseResult.found || baseResult.path.empty()) {
            continueLoop = promptAnotherSearch();
            continue;
        }

        // --------------------------------------------------------------
        // DETOUR RECOMMENDATION & ITINERARY BUILDING
        // --------------------------------------------------------------
        printSectionLine("DETOUR RECOMMENDATION ENGINE (Travel Season: " + currentSeason + ")");

        std::vector<SelectedItineraryItem> userItinerary;

        for (const std::string& cityName : baseResult.path) {
            std::cout << "\n============================================================\n"
                      << "  CITY: " << cityName << "\n"
                      << "============================================================\n";

            auto recommendations = attractionManager.getRecommendationsForCity(cityName, currentSeason);

            if (recommendations.empty()) {
                std::cout << "  (No registered tourist attractions for " << cityName << ")\n";
                continue;
            }

            for (size_t i = 0; i < recommendations.size(); ++i) {
                const auto& rec  = recommendations[i];
                const auto& attr = rec.attraction;

                std::cout << "\n  --------------------------------------------------\n"
                          << "  " << (i + 1) << ". " << attr.name << "  [" << attr.category << "]\n"
                          << "  --------------------------------------------------\n"
                          << "  * Detour         : " << attr.detourDistance << " km\n"
                          << "  * Extra Time     : " << attr.extraTime << " min\n"
                          << "  * Extra Fuel Cost: " << std::fixed << std::setprecision(0) << attr.extraFuelCost << " PKR\n"
                          << "  * Rating         : " << std::fixed << std::setprecision(1) << attr.rating << " / 5.0\n"
                          << "  * Popularity     : " << attr.popularity << " / 100\n"
                          << "  * Best Season    : " << attr.bestSeason << "\n"
                          << "  * Opening Hours  : " << attr.openingHours << "\n"
                          << "  * Entry Fee      : " << std::fixed << std::setprecision(0) << attr.entryFee << " PKR\n"
                          << "  * Visit Duration : " << std::fixed << std::setprecision(1) << attr.visitDuration << " hours\n"
                          << "  * Description    : " << attr.description << "\n"
                          << "  * Image          : " << attr.imagePath << "\n"
                          << "  * Notes          : " << attr.importantNotes << "\n"
                          << "  --------------------------------------------------\n"
                          << "  >>> Trip Value Score : " << std::fixed << std::setprecision(1) << rec.tripValueScore << " / 100\n"
                          << "  >>> Classification   : [" << rec.levelString << "]\n"
                          << "  --------------------------------------------------\n";

                if (promptAddAttraction(attr.name)) {
                    userItinerary.push_back({ cityName, rec });
                    std::cout << "  [+] Added to your custom itinerary.\n";
                } else {
                    std::cout << "  [-] Skipped.\n";
                }
            }
        }

        // --------------------------------------------------------------
        // DISPLAY FINAL CUSTOM TRAVEL ITINERARY
        // --------------------------------------------------------------
        printSectionLine("FINAL CUSTOM TRAVEL ITINERARY SUMMARY");

        double totalDetourDistance = 0.0;
        double totalExtraTimeMins  = 0.0;
        double totalExtraFuelCost  = 0.0;
        double totalEntryFees      = 0.0;
        double totalVisitDuration  = 0.0;

        std::cout << "  Base Route   : ";
        for (size_t i = 0; i < baseResult.path.size(); ++i) {
            if (i > 0) std::cout << " -> ";
            std::cout << baseResult.path[i];
        }
        std::cout << "\n\n";

        if (userItinerary.empty()) {
            std::cout << "  Selected Attractions: None (Direct Route Only)\n\n";
        } else {
            std::cout << "  Selected Tourist Attractions (" << userItinerary.size() << " total):\n";
            std::cout << "  ----------------------------------------------------------------------------------------\n";
            std::cout << "  " << std::left << std::setw(15) << "City"
                      << std::left << std::setw(28) << "Attraction"
                      << std::left << std::setw(14) << "Detour (km)"
                      << std::left << std::setw(14) << "Visit (hrs)"
                      << std::left << std::setw(14) << "Entry Fee"
                      << "Classification\n";
            std::cout << "  ----------------------------------------------------------------------------------------\n";

            for (const auto& item : userItinerary) {
                const auto& attr = item.recommendation.attraction;

                totalDetourDistance += attr.detourDistance;
                totalExtraTimeMins  += attr.extraTime;
                totalExtraFuelCost  += attr.extraFuelCost;
                totalEntryFees      += attr.entryFee;
                totalVisitDuration  += attr.visitDuration;

                std::cout << "  " << std::left << std::setw(15) << item.cityName
                          << std::left << std::setw(28) << attr.name
                          << std::left << std::setw(14) << attr.detourDistance
                          << std::left << std::setw(14) << attr.visitDuration
                          << "PKR " << std::left << std::setw(10) << static_cast<int>(attr.entryFee)
                          << "[" << item.recommendation.levelString << "]\n";
            }
            std::cout << "  ----------------------------------------------------------------------------------------\n\n";
        }

        // Calculations for updated totals
        double grandTotalDistance  = baseResult.totalDistance + totalDetourDistance;
        double extraDrivingHours   = totalExtraTimeMins / 60.0;
        double grandTotalDuration  = baseResult.totalTime + extraDrivingHours + totalVisitDuration;
        double grandTotalFuelCost  = baseResult.totalFuel + totalExtraFuelCost;
        double grandTotalFinancial = grandTotalFuelCost + totalEntryFees;

        int durHours = static_cast<int>(grandTotalDuration);
        int durMins  = static_cast<int>((grandTotalDuration - durHours) * 60.0);

        std::cout << std::fixed;
        std::cout << "  ============================================================\n"
                  << "                 RECALCULATED TRIP TOTALS                     \n"
                  << "  ============================================================\n"
                  << "  Base Route Distance     : " << std::setprecision(1) << baseResult.totalDistance << " km\n"
                  << "  Total Detour Distance   : " << std::setprecision(1) << totalDetourDistance << " km\n"
                  << "  GRAND TOTAL DISTANCE    : " << std::setprecision(1) << grandTotalDistance << " km\n"
                  << "  ------------------------------------------------------------\n"
                  << "  Base Travel Time        : " << std::setprecision(1) << baseResult.totalTime << " hours\n"
                  << "  Extra Driving Time      : " << std::setprecision(1) << extraDrivingHours << " hours (" << static_cast<int>(totalExtraTimeMins) << " mins)\n"
                  << "  Total Visit Duration    : " << std::setprecision(1) << totalVisitDuration << " hours\n"
                  << "  GRAND TOTAL TRIP TIME   : " << std::setprecision(1) << grandTotalDuration << " hours ("
                  << durHours << "h " << durMins << "m)\n"
                  << "  ------------------------------------------------------------\n"
                  << "  Base Fuel Cost          : PKR " << std::setprecision(0) << baseResult.totalFuel << "\n"
                  << "  Detour Fuel Cost        : PKR " << std::setprecision(0) << totalExtraFuelCost << "\n"
                  << "  Total Fuel Cost         : PKR " << std::setprecision(0) << grandTotalFuelCost << "\n"
                  << "  Total Entry Fees        : PKR " << std::setprecision(0) << totalEntryFees << "\n"
                  << "  GRAND TOTAL TRIP COST   : PKR " << std::setprecision(0) << grandTotalFinancial << "\n"
                  << "  ============================================================\n\n";

        continueLoop = promptAnotherSearch();
    }

    std::cout << "\n  Thank you for using PakTour Planner. Safe travels!\n\n";
    return 0;
}
