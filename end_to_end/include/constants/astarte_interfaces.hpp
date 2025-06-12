// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string_view>

namespace astarte_interfaces {
// Datastream
constexpr std::string_view DEVICE_DATASTREAM =
    "org.astarte-platform.cpp.end-to-end.DeviceDatastream";
constexpr std::string_view SERVER_DATASTREAM =
    "org.astarte-platform.cpp.end-to-end.ServerDatastream";

// Property
constexpr std::string_view DEVICE_PROPERTY = "org.astarte-platform.cpp.end-to-end.DeviceProperty";
constexpr std::string_view SERVER_PROPERTY = "org.astarte-platform.cpp.end-to-end.ServerProperty";

// Aggregate
constexpr std::string_view DEVICE_AGGREGATE = "org.astarte-platform.cpp.end-to-end.DeviceAggregate";
constexpr std::string_view SERVER_AGGREGATE = "org.astarte-platform.cpp.end-to-end.ServerAggregate";
}  // namespace astarte_interfaces
