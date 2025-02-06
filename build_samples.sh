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
    echo "  --no_system_grpc   Do not use the system gRPC but build it from scratch."
    echo "  -h, --help         Display this help message."
}

# Set the fresh mode to off by default
fresh_mode=false
no_system_grpc=false

# Check for flags
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --fresh) fresh_mode=true ;;
        --no_system_grpc) no_system_grpc=true ;;
        -h|--help) display_help; exit 0 ;;
        *) echo "Unknown option: $1"; display_help; exit 1 ;;
    esac
    shift
done

if [[ ! -d "samples/build" ]]; then
    mkdir -p samples/build
fi

cd samples/build

cmake_options=""
if [ "$no_system_grpc" != "true" ]; then
    cmake_options="-DUSE_SYSTEM_GRPC=ON"
fi

if $fresh_mode; then
    cmake --fresh $cmake_options ..
    rm -r lib_build
else
    cmake  $cmake_options ..
fi

make
