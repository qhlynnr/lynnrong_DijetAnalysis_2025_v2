#!/bin/bash

CPP_FILE="$1"   # C++ source file
EXECUTABLE="${CPP_FILE%.cpp}.out" # Executable name (same as source file without .cpp)

# Compile the C++ program
g++ "$CPP_FILE" -o "$EXECUTABLE" -std=c++20 \
    $(pythia8-config --cxxflags --libs) \
    $(fastjet-config --cxxflags --libs --plugins) \
    $(root-config --cflags --libs)

# Check if compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
fi

echo "Compilation successful. Running $EXECUTABLE..."

# Run the executable
./"$EXECUTABLE"

# Check if execution was successful
if [ $? -ne 0 ]; then
    echo "Execution failed."
    exit 2
fi

echo "Execution completed successfully."

