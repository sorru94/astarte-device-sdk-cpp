#!/bin/bash

# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

# --- Configuration ---
check_only=false
venv_dir=".venv"
clang_format_package_name="clang-format"
clang_format_package_version="19.1.6"

# --- Helper Functions ---
display_help() {
    cat << EOF
Usage: $0 [OPTIONS]

Description:
  This script checks or applies clang-format to specified C++ source and header files.

Options:
  --check-only    Run clang-format in check mode (outputs diff and errors but does not modify files).
  -h, --help      Show this help message and exit.
EOF
}

error_exit() {
    echo "Error: $1" >&2
    exit 1
}

# --- Argument Parsing ---
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --check-only)
            check_only=true
            shift
            ;;
        -h|--help) display_help; exit 0;;
        *) display_help; error_exit "Unknown option: $1";;
    esac
done

# --- Environment and Dependency Setup ---
echo "Setting up Python environment and dependencies for clang-format..."

# Check for python3
if ! command -v python3 &> /dev/null; then
    error_exit "python3 could not be found. Please install Python 3."
fi

# Create virtual environment if it doesn't exist
if [ ! -d "$venv_dir" ]; then
    echo "Creating Python virtual environment in $venv_dir..."
    if ! python3 -m venv "$venv_dir"; then
        error_exit "Failed to create Python virtual environment."
    fi
fi

# Activate virtual environment
# shellcheck source=/dev/null
if ! source "$venv_dir/bin/activate"; then
    error_exit "Failed to activate Python virtual environment."
fi

# Upgrade pip
echo "Upgrading pip..."
if ! pip install --upgrade pip; then
    error_exit "Failed to upgrade pip."
fi

# Install or verify clang-format version
echo "Checking/installing $clang_format_package_name version $clang_format_package_version..."
installed_version=$(pip show "$clang_format_package_name" | grep Version | awk '{print $2}' || true)
if [ "$installed_version" != "$clang_format_package_version" ]; then
    echo "Installing $clang_format_package_name==$clang_format_package_version..."
    if ! pip install "$clang_format_package_name==$clang_format_package_version"; then
        error_exit "Failed to install $clang_format_package_name version $clang_format_package_version."
    fi
else
    echo "$clang_format_package_name version $clang_format_package_version is already installed."
fi

# --- File Formatting Logic ---

# Define file patterns to format/check
file_patterns=(
    "src/"*.cpp
    "include/astarte_device_sdk/"*.hpp
    "private/"*.hpp
    "samples/"*/*.cpp
    "unit/"*.cpp
    "end_to_end/src/"*.cpp
    "end_to_end/include/"*.hpp
    "end_to_end/include/constants/"*.hpp
    "end_to_end/include/testcases/"*.hpp
)

command_args=("--style=file")
if [ "$check_only" = true ]; then
    echo "Running clang-format in check mode (dry run)..."
    command_args+=("--dry-run" "-Werror")
else
    echo "Applying clang-format to files..."
    command_args+=("-i")
fi

# Run clang-format on all files at once
echo "Executing: clang-format ${command_args[*]} ${file_patterns[*]}"
if ! clang-format "${command_args[@]}" "${file_patterns[@]}"; then
    if [ "$check_only" = true ]; then
        error_exit "clang-format check failed. Some files need formatting."
    else
        error_exit "clang-format application failed."
    fi
fi

if [ "$check_only" = true ]; then
    echo "Clang-format check passed. All specified files are correctly formatted."
else
    echo "Clang-format applied successfully."
fi
