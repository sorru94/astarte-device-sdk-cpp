// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_OBJECT_H
#define ASTARTE_DEVICE_SDK_OBJECT_H

/**
 * @file astarte_device_sdk/object.hpp
 * @brief Astarte object class and its related methods.
 */

#include <initializer_list>
#include <string>
#include <unordered_map>

#include "astarte_device_sdk/individual.hpp"

namespace AstarteDeviceSdk {

/** @brief Astarte object class, used for transmission of object data streams. */
class AstarteObject {
 public:
  using MapType = std::unordered_map<std::string, AstarteIndividual>;
  using iterator = MapType::iterator;
  using const_iterator = MapType::const_iterator;
  using size_type = MapType::size_type;
  using value_type = MapType::value_type;

  AstarteObject();
  AstarteObject(std::initializer_list<value_type> init);
  auto at(const std::string& key) -> AstarteIndividual&;
  auto at(const std::string& key) const -> const AstarteIndividual&;
  auto begin() -> iterator;
  auto begin() const -> const_iterator;
  auto end() -> iterator;
  auto end() const -> const_iterator;
  auto size() const -> size_type;
  auto empty() const -> bool;
  void insert(const std::string& key, const AstarteIndividual& individual);
  auto erase(const std::string& key) -> size_type;
  void clear();
  auto find(const std::string& key) -> iterator;
  auto find(const std::string& key) const -> const_iterator;
  auto format() const -> std::string;

 private:
  MapType data_;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_OBJECT_H
