// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_MSG_H
#define ASTARTE_DEVICE_SDK_MSG_H

/**
 * @file astarte_device_sdk/msg.hpp
 * @brief Astarte message class and its related methods.
 */

#include <optional>
#include <string>
#include <variant>

#include "astarte_device_sdk/individual.hpp"
#include "astarte_device_sdk/object.hpp"

/** @brief Umbrella namespace for the Astarte device SDK */
namespace AstarteDeviceSdk {

class AstarteMessage {
 public:
  AstarteMessage(std::string interface, std::string path,
                 std::optional<std::variant<AstarteIndividual, AstarteObject>> data);

  [[nodiscard]] auto get_interface() const -> const std::string&;
  [[nodiscard]] auto get_path() const -> const std::string&;
  [[nodiscard]] auto into() const
      -> const std::optional<std::variant<AstarteIndividual, AstarteObject>>&;
  [[nodiscard]] auto format() const -> std::string;

 private:
  std::string interface_;
  std::string path_;
  std::optional<std::variant<AstarteIndividual, AstarteObject>> data_;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_MSG_H
