<!--
Copyright 2025 SECO Mind Srl

SPDX-License-Identifier: Apache-2.0
-->

# gRPC sample for the Astarte device

This sample shows some basic functionality for the Astarte device library for C++.

## Configuration

The sample is already configured to connect to an Astarte message hub running on the same machine.
The Astarte message hub should be available on `localhost` at port `50051`.

## Building and running

To build this sample run the `build_sample.sh` present in the root of this project with your
preferred options. Run `build_sample.sh --help` for more info.

Running the sample should be done by excecuting the binary directly.
The binary generated by `build_sample.sh` can be found at `samples/simple/build/app`.
