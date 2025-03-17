#!/bin/bash

# Check if correct number of arguments are provided
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <cpp_file> <input_file> <output_file>"
    exit 1
fi

# Assign arguments to variables
CPP_FILE="$1"
INPUT_FILE="$2"
OUTPUT_FILE="$3"
EXECUTABLE="${CPP_FILE%.cpp}"  # Remove .cpp to name the executable

# Compile the C++ file
g++ "$CPP_FILE" -o "$EXECUTABLE" -std=c++20 $(fastjet-config --cxxflags --libs --plugins) $(root-config --cflags --libs)
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
fi

# Run the executable with input and output files
./"$EXECUTABLE" "$INPUT_FILE" "$OUTPUT_FILE"
if [ $? -ne 0 ]; then
    echo "Execution failed."
    exit 1
fi

# Clean up by removing the executable
rm -f "$EXECUTABLE"
echo "Execution completed successfully. Output saved to $OUTPUT_FILE."

