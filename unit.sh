#!/bin/bash

# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

# Function to display help message
display_help() {
    echo "Usage: $0 [OPTIONS]"
    echo
    echo "Options:"
    echo "  --stdcpp           Specify the C++ standard to use (17 or 20)"
    echo "  -h, --help         Display this help message."
}

# Set the fresh mode to off by default
cpp_standard=20

# Check for flags
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --stdcpp)
            shift
            if [[ "$1" == "17" ]]; then
                cpp_standard="17"
            elif [[ "$1" == "20" ]]; then
                cpp_standard="20"
            else
                echo "Invalid argument for --stdcpp. Use 17 or 20."
                exit 1
            fi
            ;;
        -h|--help) display_help; exit 0 ;;
        *) echo "Unknown option: $1"; display_help; exit 1 ;;
    esac
    shift
done

cd unit

cmake_options="-DUSE_SYSTEM_GRPC=ON -DMAKE_DEPENDENCIES_PUBLIC=ON -DCMAKE_CXX_STANDARD=$cpp_standard -DCMAKE_CXX_STANDARD_REQUIRED=ON"
cmake $cmake_options -S . -B build
cmake --build build
cd build && ctest

make
./unit_test
