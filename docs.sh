#!/bin/bash

# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

DOXYGEN_VERSION=1.13.2
CPP_STANDARD=202002L

function usage() {
    echo "Usage: $0 [--help] [--stdcpp <17|20>]"
    echo ""
    echo "Options:"
    echo "  --help       Show this help message and exit"
    echo "  --stdcpp     Specify the C++ standard to use (17 or 20)"
    exit 0
}

# Parse command-line arguments
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --help) usage ;;
        --stdcpp)
            shift
            if [[ "$1" == "17" ]]; then
                CPP_STANDARD="201703L"
            elif [[ "$1" == "20" ]]; then
                CPP_STANDARD="202002L"
            else
                echo "Invalid argument for --stdcpp. Use 17 or 20."
                exit 1
            fi
            ;;
        *)
            echo "Unknown option: $1"
            usage
            ;;
    esac
    shift
done

if [[ ! -d "doc/_build" ]]; then
    mkdir -p doc/_build
fi

pushd doc
pushd _build

if [[ ! -d "doxygen-${DOXYGEN_VERSION}/bin" ]]; then
    wget --no-verbose "https://github.com/doxygen/doxygen/releases/download/Release_${DOXYGEN_VERSION//./_}/doxygen-${DOXYGEN_VERSION}.linux.bin.tar.gz"
    tar xf doxygen-${DOXYGEN_VERSION}.linux.bin.tar.gz
fi

DOXYGEN_DIR="${PWD}/doxygen-${DOXYGEN_VERSION}/bin"
DOXYGEN_BIN="${DOXYGEN_DIR}/doxygen"

popd

export DOXYGEN_ASTARTE_DEVICE_SDK_VERSION="0.5.0"
export DOXYGEN_OUT="./_build"
export DOXYGEN_ASTARTE_DEVICE_SDK_BASE="$PWD/.."
export DOXYGEN_ASTARTE_DEVICE_SDK_MACROS="__cplusplus=$CPP_STANDARD"

${DOXYGEN_BIN} astarte-device-sdk.doxyfile.in

popd
