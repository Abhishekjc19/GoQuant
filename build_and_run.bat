@echo off
echo Creating build directory...
if not exist build mkdir build
cd build

echo Configuring project with CMake...
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
if %errorlevel% neq 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

echo Building project...
cmake --build .
if %errorlevel% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo Running GoQuant...
.\bin\GoQuant.exe
if %errorlevel% neq 0 (
    echo Program execution failed!
    pause
    exit /b 1
)

pause 