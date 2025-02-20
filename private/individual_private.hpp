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

#include "astarte_device_sdk/individual.hpp"

namespace AstarteDeviceSdk {

using gRPCAstarteDataTypeIndividual = astarteplatform::msghub::AstarteDataTypeIndividual;

class AstarteIndividualToAstarteDataTypeIndividual {
 public:
  auto operator()(int32_t value) -> gRPCAstarteDataTypeIndividual *;
  auto operator()(int64_t value) -> gRPCAstarteDataTypeIndividual *;
  auto operator()(double value) -> gRPCAstarteDataTypeIndividual *;
  auto operator()(bool value) -> gRPCAstarteDataTypeIndividual *;
  auto operator()(const std::string &value) -> gRPCAstarteDataTypeIndividual *;
  auto operator()(const std::vector<uint8_t> &value) -> gRPCAstarteDataTypeIndividual *;
  auto operator()(std::chrono::system_clock::time_point value) -> gRPCAstarteDataTypeIndividual *;
  auto operator()(const std::vector<int32_t> &values) -> gRPCAstarteDataTypeIndividual *;
  auto operator()(const std::vector<int64_t> &values) -> gRPCAstarteDataTypeIndividual *;
  auto operator()(const std::vector<double> &values) -> gRPCAstarteDataTypeIndividual *;
  auto operator()(const std::vector<bool> &values) -> gRPCAstarteDataTypeIndividual *;
  auto operator()(const std::vector<std::string> &values) -> gRPCAstarteDataTypeIndividual *;
  auto operator()(const std::vector<std::vector<uint8_t>> &values)
      -> gRPCAstarteDataTypeIndividual *;
  auto operator()(const std::vector<std::chrono::system_clock::time_point> &values)
      -> gRPCAstarteDataTypeIndividual *;
};

class AstarteDataTypeIndividualToAstarteIndividual {
 public:
  auto operator()(const gRPCAstarteDataTypeIndividual &value) -> AstarteIndividual;
};

}  // namespace AstarteDeviceSdk

#endif  // INDIVIDUAL_PRIVATE_H
