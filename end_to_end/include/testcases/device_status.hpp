// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "case.hpp"
#include "constants/astarte_interfaces.hpp"

namespace testcases {
TestCase device_status() {
  return TestCase(
      "Device status",
      std::vector<std::shared_ptr<TestAction>>{
          TestActionConnect::Create(), TestActionSleep::Create(std::chrono::seconds(1)),
          TestActionCheckDeviceStatus::Create(
              true, std::vector<std::string>{std::string(astarte_interfaces::DEVICE_DATASTREAM),
                                             std::string(astarte_interfaces::SERVER_DATASTREAM),
                                             std::string(astarte_interfaces::DEVICE_AGGREGATE),
                                             std::string(astarte_interfaces::SERVER_AGGREGATE),
                                             std::string(astarte_interfaces::DEVICE_PROPERTY),
                                             std::string(astarte_interfaces::SERVER_PROPERTY)}),
          TestActionDisconnect::Create(), TestActionSleep::Create(std::chrono::seconds(1))});
}
}  // namespace testcases
