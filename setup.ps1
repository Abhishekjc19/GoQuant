# Check if running as administrator
if (-NOT ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")) {
    Write-Warning "Please run this script as Administrator!"
    exit
}

# Install Chocolatey if not installed
if (!(Get-Command choco -ErrorAction SilentlyContinue)) {
    Write-Host "Installing Chocolatey..."
    Set-ExecutionPolicy Bypass -Scope Process -Force
    [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
    iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))
}

# Install CMake
Write-Host "Installing CMake..."
choco install cmake -y

# Install Git if not installed
if (!(Get-Command git -ErrorAction SilentlyContinue)) {
    Write-Host "Installing Git..."
    choco install git -y
}

# Refresh environment variables
$env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")

Write-Host "Setup complete! Please install Qt6 manually from https://www.qt.io/download-qt-installer"
Write-Host "After installing Qt6, run install_deps.bat to install nlohmann-json"
pause 