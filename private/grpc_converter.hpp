// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef GRPC_CONVERTER_H
#define GRPC_CONVERTER_H

#include <astarteplatform/msghub/astarte_message.pb.h>
#include <astarteplatform/msghub/astarte_type.pb.h>

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

#include "astarte_device_sdk/individual.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"

namespace AstarteDeviceSdk {

using gRPCAstarteDataTypeIndividual = astarteplatform::msghub::AstarteDataTypeIndividual;
using gRPCAstarteDataTypeObject = astarteplatform::msghub::AstarteDataTypeObject;
using gRPCAstarteMessage = astarteplatform::msghub::AstarteMessage;

class GrpcConverter {
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

  auto operator()(const AstarteObject &value) -> gRPCAstarteDataTypeObject *;

  auto operator()(const gRPCAstarteDataTypeIndividual &value) -> AstarteIndividual;
  auto operator()(const gRPCAstarteDataTypeObject &value) -> AstarteObject;
  auto operator()(const gRPCAstarteMessage &value) -> AstarteMessage;
};

}  // namespace AstarteDeviceSdk

#endif  // GRPC_CONVERTER_H
