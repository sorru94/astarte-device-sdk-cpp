#!/bin/bash

# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

error_exit() {
    echo "Error: $1" >&2
    exit 1
}

# Builds the Conan package from the provided directory and then builds a specified sample application.
# It handles profile detection and passes necessary build options.
#
# Arguments:
#   $1: lib_src_dir - The path to the library source directory.
#   $2: sample_to_build - The name of the sample to build.
#   $3: transport - The transport system to use. Choice between 'grpc' and 'mqtt'.
#   $4: qt_version (optional) - The major version of Qt to use if the sample is uses Qt.
#
# Example Usage:
#   build_conan_project_and_sample $(pwd) "grpc/qt" "grpc" "6"
#
build_sample_with_conan() {
    local lib_src_dir="$1"
    local sample_to_build="$2"
    local transport="$3"
    local qt_version="$4"

    # --- Argument Validation ---
    if [ -z "$lib_src_dir" ]; then
        error_exit "Library source directory not provided."
    fi
    if [ -z "$sample_to_build" ]; then
        error_exit "Sample to build not provided."
    fi
    if [[ "$transport" != "grpc" && "$transport" != "mqtt" ]]; then
        error_exit "Transport should one of: 'grpc' or 'mqtt'."
    fi
    if [[ "$sample_to_build" == "grpc/qt" && -z "$qt_version" ]]; then
        error_exit "Qt version must be provided for the 'qt' sample."
    fi

    # Detect the default conan profile
    echo "Detecting Conan profile..."
    if ! conan profile detect --exist-ok; then
        error_exit "Conan profile detection failed."
    fi

    # --- Run conan create on the library ---
    echo "Creating the library using Conan..."

    local conan_options_array=()
    conan_options_array+=("--settings=build_type=Debug")
    conan_options_array+=("--settings=compiler.cppstd=20")
    if ! conan create "${lib_src_dir}" --build=missing --options=transport=$transport "${conan_options_array[@]}"; then
        error_exit "Conan package creation failed for the library."
    fi

    # --- Run conan build on the sample ---
    echo "Running Conan for $sample_to_build sample..."

    # Enter the sample folder
    local sample_src_dir="${lib_src_dir}/samples/${sample_to_build}"
    cd "${sample_src_dir}" || error_exit "Failed to navigate to $sample_src_dir"

	# Build the sample, adding specific options if needed
    conan_options_array+=("--build=missing")
    if [[ "$sample_to_build" == "grpc/qt" ]]; then
        conan_options_array+=("--options=qt6=$([ "$qt_version" == "6" ] && echo "True" || echo "False")")
    fi

	if ! conan build . --output-folder=build --options=astarte-device-sdk/*:transport=$transport "${conan_options_array[@]}"; then
        error_exit "Conan build failed for $sample_to_build sample."
    fi

    # Return to the original directory to avoid side effects
    cd - || error_exit "Failed to return to the original directory."
    echo "Conan build process complete."
}
