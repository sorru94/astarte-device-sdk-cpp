#!/usr/bin/env bash

# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

set -exEuo pipefail

if [[ "$#" != 1 ]]; then
    echo "Missing grpc version (e.g. '1.69.0')"
    echo
    echo "Usage: $0 <grpc version>"
    exit 1
fi

GRPC_VERSION=$1

# Install gRPC dependencies
sudo apt update
sudo apt install -y build-essential autoconf libtool pkg-config ninja-build

# Clone the repository with sub-modules only if not cached
if [[ ! -d grpc ]]; then
    git clone --recurse-submodules -b "v$GRPC_VERSION" --depth 1 --shallow-submodules https://github.com/grpc/grpc
fi

MY_INSTALL_DIR=$PWD/grpc-install
mkdir -p "$MY_INSTALL_DIR"
# Export install dir
echo "MY_INSTALL_DIR=$MY_INSTALL_DIR" >>"$GITHUB_ENV"
# Export library and binary install
echo "LD_LIBRARY_PATH=$MY_INSTALL_DIR/lib:$MY_INSTALL_DIR/lib64" >>"$GITHUB_ENV"
echo "$MY_INSTALL_DIR/bin" >>"$GITHUB_PATH"
