@echo off
echo Installing dependencies...

:: Create vcpkg directory if it doesn't exist
if not exist vcpkg (
    echo Cloning vcpkg...
    git clone https://github.com/Microsoft/vcpkg.git
    cd vcpkg
    .\bootstrap-vcpkg.bat
    .\vcpkg integrate install
    .\vcpkg install nlohmann-json:x64-windows
    cd ..
)

:: Create vcpkg.json if it doesn't exist
if not exist vcpkg.json (
    echo Creating vcpkg.json...
    echo {> vcpkg.json
    echo   "name": "goquant",>> vcpkg.json
    echo   "version": "1.0.0",>> vcpkg.json
    echo   "dependencies": [>> vcpkg.json
    echo     "nlohmann-json">> vcpkg.json
    echo   ]>> vcpkg.json
    echo }>> vcpkg.json
)

echo Dependencies installed successfully!
pause 