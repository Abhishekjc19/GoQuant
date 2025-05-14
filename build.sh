#!/bin/bash

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure and build the project
cmake ..
cmake --build .

# Run the executable
./GoQuant 