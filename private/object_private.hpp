// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef OBJECT_PRIVATE_H
#define OBJECT_PRIVATE_H

#include <astarteplatform/msghub/astarte_type.pb.h>

#include "astarte_device_sdk/object.hpp"

namespace AstarteDeviceSdk {

using gRPCAstarteDataTypeObject = astarteplatform::msghub::AstarteDataTypeObject;

class AstarteObjectToAstarteDataTypeObject {
 public:
  auto operator()(const AstarteObject &value) -> gRPCAstarteDataTypeObject *;
};

class AstarteDataTypeObjectToAstarteObject {
 public:
  auto operator()(const gRPCAstarteDataTypeObject &value) -> AstarteObject;
};

}  // namespace AstarteDeviceSdk

#endif  // OBJECT_PRIVATE_H
