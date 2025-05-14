cmake_minimum_required(VERSION 3.15)
project(GoQuant VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find required packages
find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(nlohmann_json REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Source files
set(SOURCES
    src/main.cpp
    src/core/OrderBookProcessor.cpp
    src/core/FeeCalculator.cpp
    src/models/RegressionModels.cpp
    src/utils/PerformanceMonitor.cpp
)

# Header files
set(HEADERS
    include/core/OrderBookProcessor.h
    include/core/FeeCalculator.h
    include/models/RegressionModels.h
    include/utils/PerformanceMonitor.h
)

# Create executable
add_executable(GoQuant ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(GoQuant PRIVATE
    Qt6::Core
    nlohmann_json::nlohmann_json
)

# Set output directories
set_target_properties(GoQuant PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# Install
install(TARGETS GoQuant
    RUNTIME DESTINATION bin
)

# Add tests
enable_testing()
add_subdirectory(tests) 