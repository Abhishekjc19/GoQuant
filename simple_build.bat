@echo off
echo Building Simple GoQuant Trade Simulator...

REM Create build directory
if not exist simple_build mkdir simple_build
cd simple_build

REM Configure with CMake
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug

REM Build the project
cmake --build .

REM Run the executable
echo.
echo Running Simple GoQuant Trade Simulator...
echo.
.\GoQuantSimple.exe

cd ..
pause 