@echo off
echo Current directory: %CD%
echo Checking for source file...
if not exist simple_trade_sim.cpp (
    echo Error: simple_trade_sim.cpp not found!
    pause
    exit /b 1
)

echo Checking MinGW installation...
where g++ >nul 2>nul
if %errorlevel% neq 0 (
    echo Error: g++ not found! Please make sure MinGW is installed and in your PATH.
    pause
    exit /b 1
)

echo Compiling simple trade simulator...
g++ -std=c++17 -Wall -Wextra simple_trade_sim.cpp -o simple_trade_sim.exe
if %errorlevel% neq 0 (
    echo Compilation failed! Please make sure MinGW is installed and in your PATH.
    pause
    exit /b 1
)

echo Checking if executable was created...
if not exist simple_trade_sim.exe (
    echo Error: Executable was not created!
    pause
    exit /b 1
)

echo Running simulator...
echo ----------------------------------------
simple_trade_sim.exe
echo ----------------------------------------
if %errorlevel% neq 0 (
    echo Program execution failed with error code %errorlevel%!
    pause
    exit /b 1
)

echo.
echo Program completed successfully!
pause 