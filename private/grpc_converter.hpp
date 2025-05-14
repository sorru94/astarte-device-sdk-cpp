// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef GRPC_CONVERTER_H
#define GRPC_CONVERTER_H

#include <astarteplatform/msghub/astarte_data.pb.h>
#include <astarteplatform/msghub/astarte_message.pb.h>

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"

namespace AstarteDeviceSdk {

using gRPCAstarteData = astarteplatform::msghub::AstarteData;
using gRPCAstarteDatastreamIndividual = astarteplatform::msghub::AstarteDatastreamIndividual;
using gRPCAstarteDatastreamObject = astarteplatform::msghub::AstarteDatastreamObject;
using gRPCAstartePropertyIndividual = astarteplatform::msghub::AstartePropertyIndividual;
using gRPCAstarteMessage = astarteplatform::msghub::AstarteMessage;

class GrpcConverter {
 public:
  auto operator()(int32_t value) -> gRPCAstarteData *;
  auto operator()(int64_t value) -> gRPCAstarteData *;
  auto operator()(double value) -> gRPCAstarteData *;
  auto operator()(bool value) -> gRPCAstarteData *;
  auto operator()(const std::string &value) -> gRPCAstarteData *;
  auto operator()(const std::vector<uint8_t> &value) -> gRPCAstarteData *;
  auto operator()(std::chrono::system_clock::time_point value) -> gRPCAstarteData *;
  auto operator()(const std::vector<int32_t> &values) -> gRPCAstarteData *;
  auto operator()(const std::vector<int64_t> &values) -> gRPCAstarteData *;
  auto operator()(const std::vector<double> &values) -> gRPCAstarteData *;
  auto operator()(const std::vector<bool> &values) -> gRPCAstarteData *;
  auto operator()(const std::vector<std::string> &values) -> gRPCAstarteData *;
  auto operator()(const std::vector<std::vector<uint8_t>> &values) -> gRPCAstarteData *;
  auto operator()(const std::vector<std::chrono::system_clock::time_point> &values)
      -> gRPCAstarteData *;

  auto operator()(const AstarteData &value, const std::chrono::system_clock::time_point *timestamp)
      -> gRPCAstarteDatastreamIndividual *;
  auto operator()(const AstarteObject &value,
                  const std::chrono::system_clock::time_point *timestamp)
      -> gRPCAstarteDatastreamObject *;
  auto operator()(const std::optional<AstarteData> &value) -> gRPCAstartePropertyIndividual *;

  auto operator()(const gRPCAstarteData &value) -> AstarteData;
  auto operator()(const gRPCAstarteDatastreamObject &value) -> AstarteObject;
  auto operator()(const gRPCAstarteMessage &value) -> AstarteMessage;
};

}  // namespace AstarteDeviceSdk

#endif  // GRPC_CONVERTER_H
