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
./${cpp_file} bfs random 10 ;
./${cpp_file} bfs random 15 ;
./${cpp_file} bfs random 20 ;

./${cpp_file} ilp random 10 ;
./${cpp_file} ilp random 15 ;
./${cpp_file} ilp random 20 ;

./${cpp_file} eppstein random 10 ;
./${cpp_file} eppstein random 15 ;
./${cpp_file} eppstein random 20 ;

./${cpp_file} simple random 10 ;
./${cpp_file} simple random 15 ;
./${cpp_file} simple random 20 ;

./${cpp_file} heuristic random 10 ;
./${cpp_file} heuristic random 15 ;
./${cpp_file} heuristic random 20 ;





