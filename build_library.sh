#!/bin/bash

# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

# --- Configuration ---
fresh_mode=false
deps_management="system"
venv_dir=".venv"
install_lib=false

# --- Helper functions ---
display_help() {
    cat << EOF
Usage: $0 [OPTIONS]

Builds the Astarte Device SDK library.

Common options:
  --fresh        Build the library from scratch (removes its build directory).
  --deps-mgmt    Select a possible dependency management strategy. One of: system (default), fetch.
  --install      Install the library to the build/dist directory after building.
  -h, --help     Display this help message.

EOF
}

error_exit() {
    echo "Error: $1" >&2
    exit 1
}

# --- Argument Parsing ---
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --fresh) fresh_mode=true; shift ;;
        --install) install_lib=true; shift ;;
        --deps-mgmt)
            if [[ "$2" == "fetch" ]]; then
                deps_management="fetch"
            elif [[ "$2" == "system" ]]; then
                deps_management="system"
            else
                error_exit "Invalid dependency management: $2 (expected fetch or system)."
            fi
            shift 2
            ;;
        -h|--help) display_help; exit 0 ;;
        *) display_help; error_exit "Unknown option: $1" ;;
    esac
done

# --- Environment and dependency setup ---
lib_src_dir="$(pwd)"
build_dir="${lib_src_dir}/build"
cmake_user_presets="${lib_src_dir}/CMakeUserPresets.json"

# --- Clean previous builds ---
if [ "$fresh_mode" = true ]; then
    chmod +x ./scripts/clean_sample.py
    ./scripts/clean_sample.py "--build_dir=$build_dir" "--deps_mgmt=$deps_management" "--presets_file=$cmake_user_presets"
fi

# --- Perform a build with the desired dependency manager ---
if [[ "$deps_management" == "fetch" ]]; then
    chmod +x ./scripts/build_library_cmake.py
    cmd_args=()
    if [ "$install_lib" = true ]; then cmd_args+=("--install"); fi
    ./scripts/build_library_cmake.py "$(pwd)" "$(nproc --all)" "${cmd_args[@]}"
else # "system"
    chmod +x ./scripts/build_library_cmake.py
    cmd_args=()
    if [ "$install_lib" = true ]; then cmd_args+=("--install"); fi
    cmd_args+=("--system_grpc")
    ./scripts/build_library_cmake.py "$(pwd)" "$(nproc --all)" "${cmd_args[@]}"
fi

echo "Build complete for Astarte Device SDK. Output is in: $build_dir/"
