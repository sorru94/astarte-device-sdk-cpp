// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef AGGREGATE_PRIVATE_H
#define AGGREGATE_PRIVATE_H

#include <astarteplatform/msghub/astarte_type.pb.h>

#include <string>
#include <unordered_map>

#include "astarte_device_sdk/individual.h"

namespace AstarteDeviceSdk {

using astarteplatform::msghub::AstarteDataTypeObject;

class AstarteAggregateToAstarteDataTypeObject {
 public:
  auto operator()(std::unordered_map<std::string, AstarteIndividual>& value)
      -> AstarteDataTypeObject*;
};

}  // namespace AstarteDeviceSdk

#endif  // AGGREGATE_PRIVATE_H
