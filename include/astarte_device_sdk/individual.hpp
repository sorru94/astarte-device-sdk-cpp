// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_INDIVIDUAL_H
#define ASTARTE_DEVICE_SDK_INDIVIDUAL_H

/**
 * @file astarte_device_sdk/individual.hpp
 * @brief Astarte individual class and its related methods.
 */

#include <chrono>
#include <cstdint>
#include <string>
#include <variant>
#include <vector>
#if !(__cplusplus >= 202002L)
#include <type_traits>
#endif  // !(__cplusplus >= 202002L)

#include "astarte_device_sdk/type.hpp"

namespace AstarteDeviceSdk {

#if __cplusplus >= 202002L
/** @brief Restricts the allowed types for the creation of an Astarte individual class. */
template <typename T>
concept AstarteIndividualAllowedType = requires {
  requires std::is_same_v<T, int32_t> || std::is_same_v<T, int64_t> || std::is_same_v<T, double> ||
               std::is_same_v<T, bool> || std::is_same_v<T, std::string> ||
               std::is_same_v<T, std::vector<uint8_t>> ||
               std::is_same_v<T, std::chrono::system_clock::time_point> ||
               std::is_same_v<T, std::vector<int32_t>> || std::is_same_v<T, std::vector<int64_t>> ||
               std::is_same_v<T, std::vector<double>> || std::is_same_v<T, std::vector<bool>> ||
               std::is_same_v<T, std::vector<std::string>> ||
               std::is_same_v<T, std::vector<std::vector<uint8_t>>> ||
               std::is_same_v<T, std::vector<std::chrono::system_clock::time_point>>;
};
#else   // __cplusplus >= 202002L
template <typename T>
struct astarte_individual_is_allowed_type : std::false_type {};
template <>
struct astarte_individual_is_allowed_type<int32_t> : std::true_type {};
template <>
struct astarte_individual_is_allowed_type<int64_t> : std::true_type {};
template <>
struct astarte_individual_is_allowed_type<double> : std::true_type {};
template <>
struct astarte_individual_is_allowed_type<bool> : std::true_type {};
template <>
struct astarte_individual_is_allowed_type<std::string> : std::true_type {};
template <>
struct astarte_individual_is_allowed_type<std::vector<uint8_t>> : std::true_type {};
template <>
struct astarte_individual_is_allowed_type<std::chrono::system_clock::time_point> : std::true_type {
};
template <>
struct astarte_individual_is_allowed_type<std::vector<int32_t>> : std::true_type {};
template <>
struct astarte_individual_is_allowed_type<std::vector<int64_t>> : std::true_type {};
template <>
struct astarte_individual_is_allowed_type<std::vector<double>> : std::true_type {};
template <>
struct astarte_individual_is_allowed_type<std::vector<bool>> : std::true_type {};
template <>
struct astarte_individual_is_allowed_type<std::vector<std::string>> : std::true_type {};
template <>
struct astarte_individual_is_allowed_type<std::vector<std::vector<uint8_t>>> : std::true_type {};
template <>
struct astarte_individual_is_allowed_type<std::vector<std::chrono::system_clock::time_point>>
    : std::true_type {};
#endif  // __cplusplus >= 202002L

/** @brief Astarte individual object, used for transmission of individual data streams. */
class AstarteIndividual {
 public:
#if __cplusplus >= 202002L
  template <AstarteIndividualAllowedType T>
  explicit AstarteIndividual(T value) : data_(value) {}

  template <AstarteIndividualAllowedType T>
  auto into() const -> const T& {
    return std::get<T>(data_);
  }
#else   // __cplusplus >= 202002L
  template <typename T>
  explicit AstarteIndividual(
      T value,
      std::enable_if_t<astarte_individual_is_allowed_type<T>::value, bool> /*unused*/ = true)
      : data_(value) {}

  template <typename T>
  auto into(std::enable_if_t<astarte_individual_is_allowed_type<T>::value, bool> /*unused*/ =
                true) const -> const T& {
    return std::get<T>(data_);
  }
#endif  // __cplusplus >= 202002L

  [[nodiscard]] auto format() const -> std::string;
  [[nodiscard]] auto get_type() const -> AstarteType;
  [[nodiscard]] auto get_data() const
      -> const std::variant<int32_t, int64_t, double, bool, std::string, std::vector<uint8_t>,
                            std::chrono::system_clock::time_point, std::vector<int32_t>,
                            std::vector<int64_t>, std::vector<double>, std::vector<bool>,
                            std::vector<std::string>, std::vector<std::vector<uint8_t>>,
                            std::vector<std::chrono::system_clock::time_point>>&;

 private:
  std::variant<int32_t, int64_t, double, bool, std::string, std::vector<uint8_t>,
               std::chrono::system_clock::time_point, std::vector<int32_t>, std::vector<int64_t>,
               std::vector<double>, std::vector<bool>, std::vector<std::string>,
               std::vector<std::vector<uint8_t>>,
               std::vector<std::chrono::system_clock::time_point>>
      data_;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_INDIVIDUAL_H
