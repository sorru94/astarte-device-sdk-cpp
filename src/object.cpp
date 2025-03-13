// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/object.hpp"

#include <initializer_list>
#include <sstream>
#include <string>
#include <utility>

#include "astarte_device_sdk/individual.hpp"

namespace AstarteDeviceSdk {

// Default constructor
AstarteObject::AstarteObject() = default;
// Constructor with initializer list
AstarteObject::AstarteObject(std::initializer_list<MapType::value_type> init) : data_(init) {}
// Access element by key (modifiable)
auto AstarteObject::at(const std::string& key) -> AstarteIndividual& { return data_.at(key); }
// Access element by key (const)
auto AstarteObject::at(const std::string& key) const -> const AstarteIndividual& {
  return data_.at(key);
}
// Begin iterator (modifiable)
auto AstarteObject::begin() -> iterator { return data_.begin(); }
// Begin iterator (const)
auto AstarteObject::begin() const -> const_iterator { return data_.begin(); }
// End iterator (modifiable)
auto AstarteObject::end() -> iterator { return data_.end(); }
// End iterator (const)
auto AstarteObject::end() const -> const_iterator { return data_.end(); }
// Get size of the map
auto AstarteObject::size() const -> size_type { return data_.size(); }
// Check if map is empty
auto AstarteObject::empty() const -> bool { return data_.empty(); }
// Insert element into the map
void AstarteObject::insert(const std::string& key, const AstarteIndividual& individual) {
  data_.insert({key, individual});
}
// Erase element by key
auto AstarteObject::erase(const std::string& key) -> size_type { return data_.erase(key); }
// Clear the map
void AstarteObject::clear() { data_.clear(); }
// Find element by key (modifiable)
auto AstarteObject::find(const std::string& key) -> iterator { return data_.find(key); }
// Find element by key (const)
auto AstarteObject::find(const std::string& key) const -> const_iterator { return data_.find(key); }
// Format the map content
auto AstarteObject::format() const -> std::string {
  std::ostringstream oss;
  bool first = true;
  for (const auto& pair : data_) {
    if (!first) {
      oss << ", ";
    }
    oss << "\"" << pair.first << "\": " << pair.second.format();
    first = false;
  }
  return oss.str();
}

auto AstarteObject::get_raw_data() const -> const MapType& { return this->data_; }

auto AstarteObject::operator==(const AstarteObject& other) const -> bool {
  return this->data_ == other.get_raw_data();
}
auto AstarteObject::operator!=(const AstarteObject& other) const -> bool {
  return this->data_ != other.get_raw_data();
}

}  // namespace AstarteDeviceSdk
