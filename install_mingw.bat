@echo off
echo Installing MinGW...

:: Create a temporary directory
mkdir temp
cd temp

:: Download MinGW installer
echo Downloading MinGW installer...
powershell -Command "& {Invoke-WebRequest -Uri 'https://github.com/niXman/mingw-builds-binaries/releases/download/13.2.0-rt_v11-rev1/x86_64-13.2.0-release-posix-seh-ucrt-rt_v11-rev1.7z' -OutFile 'mingw.7z'}"

:: Extract MinGW
echo Extracting MinGW...
powershell -Command "& {Expand-Archive -Path 'mingw.7z' -DestinationPath 'C:\'}"

:: Add MinGW to PATH
echo Adding MinGW to PATH...
setx PATH "%PATH%;C:\mingw64\bin"

:: Clean up
cd ..
rmdir /s /q temp

echo MinGW installation complete!
echo Please restart your command prompt and try running the program again.
pause 