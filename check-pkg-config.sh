#!/bin/bash

# (C) Copyright 2026, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

export PKG_CONFIG_PATH=$(pwd)/build/dist/lib/pkgconfig:/home/work/.local/lib/pkgconfig:/home/work/Documents/repos/grpc/third_party/re2

echo "Validating astarte_device_sdk pkg-config file:"
pkg-config --print-errors --validate astarte_device_sdk

echo "Checking the public dependencies of Astarte Device SDK:"
pkg-config --print-requires astarte_device_sdk

echo "Checking the private dependencies of Astarte Device SDK:"
pkg-config --print-requires-private astarte_device_sdk

echo "Cflags and Libs for Astarte Device SDK:"
pkg-config --cflags --libs astarte_device_sdk

echo "Compile a sample program:"
g++ -std=c++23 test_pkg.cpp -o test_pkg $(pkg-config --cflags --libs astarte_device_sdk)
