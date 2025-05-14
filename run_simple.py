import os
import subprocess
import sys

def run_command(command):
    print(f"Running: {command}")
    process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()
    
    if stdout:
        print("Output:")
        print(stdout.decode())
    if stderr:
        print("Errors:")
        print(stderr.decode())
    
    return process.returncode

def main():
    print("Current directory:", os.getcwd())
    
    # Check if source file exists
    if not os.path.exists("simple_trade_sim.cpp"):
        print("Error: simple_trade_sim.cpp not found!")
        return 1
    
    # Compile the program
    print("\nCompiling program...")
    compile_cmd = "g++ -std=c++17 -Wall -Wextra simple_trade_sim.cpp -o simple_trade_sim.exe"
    if run_command(compile_cmd) != 0:
        print("Compilation failed!")
        return 1
    
    # Run the program
    print("\nRunning program...")
    print("-" * 40)
    if run_command("simple_trade_sim.exe") != 0:
        print("Program execution failed!")
        return 1
    print("-" * 40)
    
    print("\nProgram completed successfully!")
    return 0

if __name__ == "__main__":
    sys.exit(main()) 