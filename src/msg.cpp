// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/msg.hpp"

#include <astarteplatform/msghub/astarte_type.pb.h>

#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <variant>

#include "astarte_device_sdk/exceptions.hpp"
#include "astarte_device_sdk/individual.hpp"
#include "astarte_device_sdk/object.hpp"
#include "individual_private.hpp"
#include "msg_private.hpp"
#include "object_private.hpp"

namespace AstarteDeviceSdk {

using gRPCAstarteDataType = astarteplatform::msghub::AstarteDataType;
using gRPCAstarteDataTypeIndividual = astarteplatform::msghub::AstarteDataTypeIndividual;

AstarteMessage::AstarteMessage(std::string interface, std::string path,
                               std::optional<std::variant<AstarteIndividual, AstarteObject>> data)
    : interface_(std::move(interface)), path_(std::move(path)), data_(std::move(data)) {}

auto AstarteMessage::get_interface() const -> const std::string & { return interface_; }

auto AstarteMessage::get_path() const -> const std::string & { return path_; }

auto AstarteMessage::into() const
    -> const std::optional<std::variant<AstarteIndividual, AstarteObject>> & {
  return data_;
}

auto AstarteMessage::format() const -> std::string {
  std::ostringstream oss;
  oss << "{interface: " << interface_ << ", path: " << path_;
  if (data_.has_value()) {
    if (std::holds_alternative<AstarteIndividual>(data_.value())) {
      const AstarteIndividual ind = std::get<AstarteIndividual>(data_.value());
      oss << ", value: " << ind.format();
    } else {
      const AstarteObject obj = std::get<AstarteObject>(data_.value());
      oss << ", value: " << obj.format();
    }
  }
  oss << "}";
  return oss.str();
}

auto AstarteMessageToAstarteMessage::operator()(const gRPCAstarteMessage &value) -> AstarteMessage {
  std::optional<std::variant<AstarteIndividual, AstarteObject>> parsed_data;
  if (value.has_astarte_data()) {
    const gRPCAstarteDataType &data = value.astarte_data();
    if (data.has_astarte_individual()) {
      const gRPCAstarteDataTypeIndividual &individual = data.astarte_individual();
      AstarteDataTypeIndividualToAstarteIndividual converter;
      parsed_data = converter(individual);
    } else if (data.has_astarte_object()) {
      AstarteDataTypeObjectToAstarteObject converter;
      parsed_data = converter(data.astarte_object());
    } else {
      throw AstarteInternalException("Found an unrecognized gRPC gRPCAstarteDataType.");
    }
  } else if (value.has_astarte_unset()) {
    parsed_data = std::nullopt;
  }
  return {value.interface_name(), value.path(), parsed_data};
}

}  // namespace AstarteDeviceSdk
