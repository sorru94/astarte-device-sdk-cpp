// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "case.hpp"
#include "constants/astarte_interfaces.hpp"

namespace testcases {
std::unique_ptr<TestCase> device_status() {
  auto device_status = std::make_unique<TestCase>(
      "Device status",
      std::vector<std::shared_ptr<TestAction>>{
          std::make_shared<TestActionConnect>(),
          std::make_shared<TestActionSleep>(std::chrono::seconds(1)),
          std::make_shared<TestActionCheckDeviceStatus>(
              true, std::vector<std::string>{std::string(astarte_interfaces::DEVICE_DATASTREAM),
                                             std::string(astarte_interfaces::SERVER_DATASTREAM),
                                             std::string(astarte_interfaces::DEVICE_AGGREGATE),
                                             std::string(astarte_interfaces::SERVER_AGGREGATE),
                                             std::string(astarte_interfaces::DEVICE_PROPERTY),
                                             std::string(astarte_interfaces::SERVER_PROPERTY)}),
          std::make_shared<TestActionDisconnect>(),
          std::make_shared<TestActionSleep>(std::chrono::seconds(1))});

  return device_status;
}
}  // namespace testcases
