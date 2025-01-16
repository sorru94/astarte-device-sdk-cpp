// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_INDIVIDUAL_PRIV_H
#define ASTARTE_INDIVIDUAL_PRIV_H

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

#include "astarteplatform/msghub/astarte_type.pb.h"

namespace AstarteDeviceSdk {

using astarteplatform::msghub::AstarteDataTypeIndividual;

struct AstarteIndividualToAstarteDataTypeIndividual {
  auto operator()(int32_t value) const -> AstarteDataTypeIndividual *;
  auto operator()(int64_t value) const -> AstarteDataTypeIndividual *;
  auto operator()(double value) const -> AstarteDataTypeIndividual *;
  auto operator()(bool value) const -> AstarteDataTypeIndividual *;
  auto operator()(std::string value) const -> AstarteDataTypeIndividual *;
  auto operator()(std::vector<uint8_t> value) const -> AstarteDataTypeIndividual *;
  auto operator()(std::chrono::system_clock::time_point value) const -> AstarteDataTypeIndividual *;
  auto operator()(const std::vector<int32_t> &values) const -> AstarteDataTypeIndividual *;
  auto operator()(const std::vector<int64_t> &values) const -> AstarteDataTypeIndividual *;
  auto operator()(const std::vector<double> &values) const -> AstarteDataTypeIndividual *;
  auto operator()(const std::vector<bool> &values) const -> AstarteDataTypeIndividual *;
  auto operator()(const std::vector<std::string> &values) const -> AstarteDataTypeIndividual *;
  auto operator()(const std::vector<std::vector<uint8_t>> &values) const
      -> AstarteDataTypeIndividual *;
  auto operator()(const std::vector<std::chrono::system_clock::time_point> &values) const
      -> AstarteDataTypeIndividual *;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_INDIVIDUAL_PRIV_H
