// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef MSG_PRIVATE_H
#define MSG_PRIVATE_H

#include <astarteplatform/msghub/astarte_message.pb.h>

#include "astarte_device_sdk/msg.hpp"

namespace AstarteDeviceSdk {

using gRPCAstarteMessage = astarteplatform::msghub::AstarteMessage;

class AstarteMessageToAstarteMessage {
 public:
  auto operator()(const gRPCAstarteMessage &value) -> AstarteMessage;
};

}  // namespace AstarteDeviceSdk

#endif  // MSG_PRIVATE_H
