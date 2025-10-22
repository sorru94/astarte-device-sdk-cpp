// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_PAIRING_H
#define ASTARTE_MQTT_PAIRING_H

#include <chrono>
#include <format>
#include <string>
#include <string_view>

#include "ada.h"
#include "exceptions.hpp"

namespace AstarteDeviceSdk {

/// using namespace for timeout values
using namespace std::chrono_literals;

/** @brief Class providing the methods to perform the Astarte pairing operations */
class PairingApi {
 public:
  /**
   * @brief Constructor for the PairingApi class.
   * @param realm The Astarte realm name.
   * @param device_id The Astarte device id.
   * @param astarte_base_url string containing the Astarte pairing API URL.
   */
  PairingApi(std::string_view realm, std::string_view device_id, std::string_view astarte_base_url)
      : realm(realm), device_id(device_id), pairing_url(create_pairing_url(astarte_base_url)) {}

  /**
   * @brief Register a device.
   * @param pairing_token The Astarte pairing token.
   * @param timeout_ms A timeout value to perform the HTTP request.
   * @return The credentials secret if successful, otherwise std::nullopt.
   */
  auto register_device(std::string_view pairing_token,
                       std::chrono::milliseconds timeout_ms = 0ms) const -> std::string;

  /** @brief The Astarte realm name. */
  const std::string realm;
  /** @brief The Astarte device id. */
  const std::string device_id;
  /** @brief The Astarte pairing URL as a string. */
  const ada::url_aggregator pairing_url;

 private:
  // static helper function used to parse astarte base URL and create the pairing URL.
  static auto create_pairing_url(std::string_view astarte_base_url) -> ada::url_aggregator;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_MQTT_PAIRING_H
