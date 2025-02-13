// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_TYPE_H
#define ASTARTE_DEVICE_SDK_TYPE_H

/**
 * @file astarte_device_sdk/type.hpp
 * @brief Types definitions for communication with Astarte.
 */

#include <cstdint>

namespace AstarteDeviceSdk {

enum AstarteType : int8_t {
  BINARYBLOB,
  BOOLEAN,
  DATETIME,
  DOUBLE,
  INTEGER,
  LONGINTEGER,
  STRING,
  BINARYBLOBARRAY,
  BOOLEANARRAY,
  DATETIMEARRAY,
  DOUBLEARRAY,
  INTEGERARRAY,
  LONGINTEGERARRAY,
  STRINGARRAY
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_TYPE_H
