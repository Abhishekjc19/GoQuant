@echo off
echo Building GoQuant Trade Simulator...

REM Create build directory
if not exist build mkdir build
cd build

REM Configure with CMake
echo.
echo Configuring with CMake...
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
if errorlevel 1 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

REM Build the project
echo.
echo Building project...
cmake --build . --verbose
if errorlevel 1 (
    echo Build failed!
    pause
    exit /b 1
)

REM Check if executable exists
if not exist GoQuant.exe (
    echo Executable not found!
    echo Current directory contents:
    dir
    pause
    exit /b 1
)

REM Run the executable
echo.
echo Running GoQuant Trade Simulator...
echo.
.\GoQuant.exe
if errorlevel 1 (
    echo Program execution failed!
    pause
    exit /b 1
)

cd ..
pause 