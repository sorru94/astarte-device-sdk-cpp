// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/msg.hpp"

#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <variant>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/object.hpp"

namespace AstarteDeviceSdk {

AstarteMessage::AstarteMessage(std::string interface, std::string path, AstarteMessageType type,
                               std::optional<std::variant<AstarteData, AstarteObject>> data)
    : interface_(std::move(interface)),
      path_(std::move(path)),
      type_(type),
      data_(std::move(data)) {}

auto AstarteMessage::get_interface() const -> const std::string & { return interface_; }

auto AstarteMessage::get_path() const -> const std::string & { return path_; }

auto AstarteMessage::get_message_type() const -> const AstarteMessageType & { return type_; }

auto AstarteMessage::into() const
    -> const std::optional<std::variant<AstarteData, AstarteObject>> & {
  return data_;
}

auto AstarteMessage::operator==(const AstarteMessage &other) const -> bool {
  return this->interface_ == other.get_interface() && this->path_ == other.get_path() &&
         this->data_ == other.into();
}
auto AstarteMessage::operator!=(const AstarteMessage &other) const -> bool {
  return this->interface_ != other.get_interface() || this->path_ != other.get_path() ||
         this->data_ != other.into();
}

auto AstarteMessage::format() const -> std::string {
  std::ostringstream oss;
  oss << "{interface: " << interface_ << ", path: " << path_;
  if (data_.has_value()) {
    if (std::holds_alternative<AstarteData>(data_.value())) {
      const AstarteData ind = std::get<AstarteData>(data_.value());
      oss << ", value: " << ind.format();
    } else {
      const AstarteObject obj = std::get<AstarteObject>(data_.value());
      oss << ", value: " << obj.format();
    }
  }
  oss << "}";
  return oss.str();
}

}  // namespace AstarteDeviceSdk
