// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/device_grpc.hpp"

#include <chrono>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "device_grpc_impl.hpp"

namespace AstarteDeviceSdk {

AstarteDeviceGRPC::AstarteDeviceGRPC(const std::string &server_addr, const std::string &node_uuid)
    : astarte_device_impl_{std::make_shared<AstarteDeviceGRPCImpl>(server_addr, node_uuid)} {}

AstarteDeviceGRPC::~AstarteDeviceGRPC() = default;

void AstarteDeviceGRPC::add_interface_from_json(const std::filesystem::path &json_file) {
  astarte_device_impl_->add_interface_from_json(json_file);
}

void AstarteDeviceGRPC::add_interface_from_str(std::string_view json) {
  astarte_device_impl_->add_interface_from_str(json);
}

void AstarteDeviceGRPC::connect() { astarte_device_impl_->connect(); }

auto AstarteDeviceGRPC::is_connected(const std::chrono::milliseconds &timeout) const -> bool {
  return astarte_device_impl_->is_connected(timeout);
}

void AstarteDeviceGRPC::disconnect() { astarte_device_impl_->disconnect(); }

void AstarteDeviceGRPC::send_individual(std::string_view interface_name, std::string_view path,
                                        const AstarteData &data,
                                        const std::chrono::system_clock::time_point *timestamp) {
  astarte_device_impl_->send_individual(interface_name, path, data, timestamp);
}

void AstarteDeviceGRPC::send_object(std::string_view interface_name, std::string_view path,
                                    const AstarteDatastreamObject &object,
                                    const std::chrono::system_clock::time_point *timestamp) {
  astarte_device_impl_->send_object(interface_name, path, object, timestamp);
}

void AstarteDeviceGRPC::set_property(std::string_view interface_name, std::string_view path,
                                     const AstarteData &data) {
  astarte_device_impl_->set_property(interface_name, path, data);
}

void AstarteDeviceGRPC::unset_property(std::string_view interface_name, std::string_view path) {
  astarte_device_impl_->unset_property(interface_name, path);
}

auto AstarteDeviceGRPC::poll_incoming(const std::chrono::milliseconds &timeout)
    -> std::optional<AstarteMessage> {
  return astarte_device_impl_->poll_incoming(timeout);
}

}  // namespace AstarteDeviceSdk
