@echo off

REM Create build directory if it doesn't exist
if not exist build mkdir build
cd build

REM Configure and build the project
cmake ..
cmake --build .

REM Run the executable
.\GoQuant.exe 