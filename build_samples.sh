#!/bin/bash

# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

# Function to display help message
display_help() {
    echo "Usage: $0 [OPTIONS]"
    echo
    echo "Options:"
    echo "  --fresh            Build the samples from scratch."
    echo "  --stdcpp           Specify the C++ standard to use (17 or 20)"
    echo "  --no_system_grpc   Do not use the system gRPC but build it from scratch."
    echo "  -h, --help         Display this help message."
}

# Set the fresh mode to off by default
fresh_mode=false
no_system_grpc=false
cpp_standard=20

# Check for flags
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --fresh) fresh_mode=true ;;
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
        --no_system_grpc) no_system_grpc=true ;;
        -h|--help) display_help; exit 0 ;;
        *) echo "Unknown option: $1"; display_help; exit 1 ;;
    esac
    shift
done

if [[ ! -d "samples/grpc/build" ]]; then
    mkdir -p samples/grpc/build
fi

cd samples/grpc/build

cmake_options=""
if [ "$no_system_grpc" != "true" ]; then
    cmake_options="-DUSE_SYSTEM_GRPC=ON"
fi

if $fresh_mode; then
    cmake --fresh $cmake_options -DCMAKE_CXX_STANDARD=$cpp_standard -DCMAKE_CXX_STANDARD_REQUIRED=ON ..
    rm -r lib_build
else
    cmake  $cmake_options -DCMAKE_CXX_STANDARD=$cpp_standard -DCMAKE_CXX_STANDARD_REQUIRED=ON ..
fi

make
