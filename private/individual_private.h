// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef INDIVIDUAL_PRIVATE_H
#define INDIVIDUAL_PRIVATE_H

#include <astarteplatform/msghub/astarte_type.pb.h>

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace AstarteDeviceSdk {

using astarteplatform::msghub::AstarteDataTypeIndividual;

class AstarteIndividualToAstarteDataTypeIndividual {
 public:
  auto operator()(int32_t value) -> AstarteDataTypeIndividual *;
  auto operator()(int64_t value) -> AstarteDataTypeIndividual *;
  auto operator()(double value) -> AstarteDataTypeIndividual *;
  auto operator()(bool value) -> AstarteDataTypeIndividual *;
  auto operator()(const std::string &value) -> AstarteDataTypeIndividual *;
  auto operator()(const std::vector<uint8_t> &value) -> AstarteDataTypeIndividual *;
  auto operator()(std::chrono::system_clock::time_point value) -> AstarteDataTypeIndividual *;
  auto operator()(const std::vector<int32_t> &values) -> AstarteDataTypeIndividual *;
  auto operator()(const std::vector<int64_t> &values) -> AstarteDataTypeIndividual *;
  auto operator()(const std::vector<double> &values) -> AstarteDataTypeIndividual *;
  auto operator()(const std::vector<bool> &values) -> AstarteDataTypeIndividual *;
  auto operator()(const std::vector<std::string> &values) -> AstarteDataTypeIndividual *;
  auto operator()(const std::vector<std::vector<uint8_t>> &values) -> AstarteDataTypeIndividual *;
  auto operator()(const std::vector<std::chrono::system_clock::time_point> &values)
      -> AstarteDataTypeIndividual *;
};

}  // namespace AstarteDeviceSdk

#endif  // INDIVIDUAL_PRIVATE_H
