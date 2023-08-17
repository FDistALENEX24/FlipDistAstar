#!/bin/bash

# Check if the build directory exists
if [ ! -d "build" ]; then
    echo "Error: 'build' directory not found. Please compile your C++ file first."
    exit 1
fi

# Check if the C++ file exists in the build directory
cpp_file="A_star_for_flipdistance"
if [ ! -f "build/${cpp_file}" ]; then
    echo "Error: C++ file '${cpp_file}' not found in the 'build' directory."
    exit 1
fi

# Execute the C++ file with each set of parameters one after the other
cd build 
./${cpp_file} eppstein random 25 ;
./${cpp_file} simple random 25 ;
./${cpp_file} heuristic random 25;

