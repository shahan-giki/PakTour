cmake_minimum_required(VERSION 3.16)

project(PakTourPlanner
    VERSION 1.3.0
    DESCRIPTION "PakTour Planner - Smart Detour Recommendation Engine & Custom Itinerary Builder"
    LANGUAGES CXX
)

# -----------------------------------------------------------------------
# C++ Standard
# -----------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -----------------------------------------------------------------------
# Compiler warnings
# -----------------------------------------------------------------------
if (MSVC)
    add_compile_options(/W4 /WX-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# -----------------------------------------------------------------------
# Source files
# -----------------------------------------------------------------------
set(SOURCES
    src/City.cpp
    src/Road.cpp
    src/Graph.cpp
    src/Dijkstra.cpp
    src/AttractionManager.cpp
    src/main.cpp
)

set(HEADERS
    include/City.h
    include/Road.h
    include/Graph.h
    include/Dijkstra.h
    include/AttractionManager.h
)

# -----------------------------------------------------------------------
# Executable
# -----------------------------------------------------------------------
add_executable(PakTourPlanner ${SOURCES} ${HEADERS})

target_include_directories(PakTourPlanner PRIVATE include)

# -----------------------------------------------------------------------
# Copy data directory to the build output directory so that the
# executable can locate data/*.csv files.
# -----------------------------------------------------------------------
add_custom_command(TARGET PakTourPlanner POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/data"
        "$<TARGET_FILE_DIR:PakTourPlanner>/data"
    COMMENT "Copying data files to build directory..."
)

# -----------------------------------------------------------------------
# Print a summary
# -----------------------------------------------------------------------
message(STATUS "---------------------------------------------")
message(STATUS "  PakTour Planner – Detour Recommendation Engine")
message(STATUS "  C++ Standard : ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build type   : ${CMAKE_BUILD_TYPE}")
message(STATUS "---------------------------------------------")
