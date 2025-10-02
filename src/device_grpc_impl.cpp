// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "device_grpc_impl.hpp"

#include <astarteplatform/msghub/astarte_data.pb.h>
#include <astarteplatform/msghub/astarte_message.pb.h>
#include <astarteplatform/msghub/interface.pb.h>
#include <astarteplatform/msghub/message_hub_error.pb.h>
#include <astarteplatform/msghub/message_hub_service.grpc.pb.h>
#include <astarteplatform/msghub/node.pb.h>
#include <astarteplatform/msghub/property.pb.h>
#include <google/protobuf/empty.pb.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/support/channel_arguments.h>
#include <grpcpp/support/client_interceptor.h>
#include <grpcpp/support/status.h>

#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <memory>
#include <optional>
#include <regex>
#include <stop_token>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/device_grpc.hpp"
#include "astarte_device_sdk/exceptions.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/property.hpp"
#include "astarte_device_sdk/stored_property.hpp"
#include "exponential_backoff.hpp"
#include "grpc_converter.hpp"
#include "grpc_interceptors.hpp"
#include "logger.hpp"
#include "shared_queue.hpp"

namespace AstarteDeviceSdk {

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::Status;

using grpc::experimental::ClientInterceptorFactoryInterface;

using gRPCAstarteData = astarteplatform::msghub::AstarteData;
using gRPCAstarteDatastreamObject = astarteplatform::msghub::AstarteDatastreamObject;
using gRPCAstartePropertyIndividual = astarteplatform::msghub::AstartePropertyIndividual;
using gRPCAstarteMessage = astarteplatform::msghub::AstarteMessage;
using gRPCMessageHub = astarteplatform::msghub::MessageHub;
using gRPCMessageHubError = astarteplatform::msghub::MessageHubError;
using gRPCMessageHubEvent = astarteplatform::msghub::MessageHubEvent;
using gRPCNode = astarteplatform::msghub::Node;
using gRPCInterfaceName = astarteplatform::msghub::InterfaceName;
using gRPCStoredProperties = astarteplatform::msghub::StoredProperties;
using gRPCOwnership = astarteplatform::msghub::Ownership;
using gRPCPropertyFilter = astarteplatform::msghub::PropertyFilter;
using gRPCPropertyIdentifier = astarteplatform::msghub::PropertyIdentifier;
using gRPCInterfacesJson = astarteplatform::msghub::InterfacesJson;
using gRPCInterfacesName = astarteplatform::msghub::InterfacesName;

AstarteDeviceGRPC::AstarteDeviceGRPCImpl::AstarteDeviceGRPCImpl(std::string server_addr,
                                                                std::string node_uuid)
    : server_addr_(std::move(server_addr)),
      node_uuid_(std::move(node_uuid)),
      connected_(std::atomic_bool(false)),
      grpc_stream_error_(std::atomic_bool(false)) {}

AstarteDeviceGRPC::AstarteDeviceGRPCImpl::~AstarteDeviceGRPCImpl() { ssource_.request_stop(); }

void AstarteDeviceGRPC::AstarteDeviceGRPCImpl::add_interface_from_file(
    const std::filesystem::path& json_file) {
  logging::debug("Adding interface from file: {}", json_file.string());

  // Check file validity
  std::ifstream interface_file(json_file, std::ios::in);
  if (!interface_file.is_open()) {
    logging::error("Could not open the interface file: {}", json_file.string());
    throw AstarteFileOpenException(json_file.string());
  }

  // Read the entire JSON file content into a string
  const std::string interface_json((std::istreambuf_iterator<char>(interface_file)),
                                   std::istreambuf_iterator<char>());

  // Close the file
  interface_file.close();

  // Add the interface from the fetched string
  add_interface_from_str(interface_json);
}

void AstarteDeviceGRPC::AstarteDeviceGRPCImpl::add_interface_from_str(std::string_view json) {
  logging::debug("Adding interface from string");

  // If the device is connected, notify the message hub
  if (is_connected()) {
    gRPCInterfacesJson grpc_interfaces_json;
    grpc_interfaces_json.add_interfaces_json(json);
    ClientContext context;
    google::protobuf::Empty response;
    const Status status = stub_->AddInterfaces(&context, grpc_interfaces_json, &response);
    if (!status.ok()) {
      logging::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
      return;
    }
  }

  interfaces_bins_.emplace_back(json);
  logging::trace("Added interface: \n{}", json);
}

void AstarteDeviceGRPC::AstarteDeviceGRPCImpl::remove_interface(const std::string& interface_name) {
  logging::debug("Removing interface: {}", interface_name);
  const std::string escaped_interface_name =
      std::regex_replace(interface_name, std::regex("\\."), "\\.");
  const std::string pattern_string =
      R"(\"interface_name\":\s*\")" + escaped_interface_name + R"(\")";
  const std::regex pattern(pattern_string);

  for (auto i = interfaces_bins_.begin(); i != interfaces_bins_.end(); ++i) {
    const std::string& interface_json = *i;
    std::smatch match;
    if (std::regex_search(interface_json, match, pattern)) {
      if (is_connected()) {
        gRPCInterfacesName grpc_interface_names;
        grpc_interface_names.add_names(interface_name);
        ClientContext context;
        google::protobuf::Empty response;
        const Status status = stub_->RemoveInterfaces(&context, grpc_interface_names, &response);
        if (!status.ok()) {
          logging::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
          return;
        }
      }
      interfaces_bins_.erase(i);
      break;
    }
  }
}

void AstarteDeviceGRPC::AstarteDeviceGRPCImpl::connect() {
  logging::info("Connection requested.");
  if (connection_thread_) {
    logging::warn("Connection process is already running.");
    return;
  }

  // create a fresh stop source for this new connection session
  ssource_ = std::stop_source();

  // start the connection loop, passing it the token from our source.
  connection_thread_.emplace([this](const std::stop_token& token) { this->connection_loop(token); },
                             ssource_.get_token());
}

auto AstarteDeviceGRPC::AstarteDeviceGRPCImpl::is_connected() const -> bool {
  return connected_.load();
}

void AstarteDeviceGRPC::AstarteDeviceGRPCImpl::disconnect() {
  logging::info("Disconnection requested.");

  // request a stop to signal connection_loop and handle_events
  ssource_.request_stop();

  if (connected_.load() || grpc_stream_error_.load()) {
    ClientContext context;
    google::protobuf::Empty response;
    const Status status = stub_->Detach(&context, google::protobuf::Empty(), &response);
    if (!status.ok()) {
      logging::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    }
    grpc_stream_error_.store(false);
  }

  // clear the thread object by invoking the destructor on the internal thread.
  // jthread's destructor will join
  connection_thread_.reset();
}

void AstarteDeviceGRPC::AstarteDeviceGRPCImpl::send_individual(
    std::string_view interface_name, std::string_view path, const AstarteData& data,
    const std::chrono::system_clock::time_point* timestamp) {
  logging::debug("Sending individual: {} {}", interface_name, path);
  if (!connected_.load()) {
    const std::string_view msg("Device disconnected, operation aborted.");
    logging::warn(msg);
    throw AstarteOperationRefusedException(msg);
  }
  gRPCAstarteMessage message;
  message.set_interface_name(interface_name);
  message.set_path(path);

  GrpcConverterTo converter;
  std::unique_ptr<gRPCAstarteDatastreamIndividual> grpc_datastream_individual =
      converter(data, timestamp);
  message.set_allocated_datastream_individual(grpc_datastream_individual.release());

  ClientContext context;
  google::protobuf::Empty response;
  logging::trace("Sending data: {} {}", interface_name, path);
  const Status status = stub_->Send(&context, message, &response);
  if (!status.ok()) {
    logging::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    throw AstarteInvalidInputException(status.error_message());
  }
}

void AstarteDeviceGRPC::AstarteDeviceGRPCImpl::send_object(
    std::string_view interface_name, std::string_view path, const AstarteDatastreamObject& object,
    const std::chrono::system_clock::time_point* timestamp) {
  logging::debug("Sending object: {} {}", interface_name, path);
  if (!connected_.load()) {
    const std::string_view msg("Device disconnected, operation aborted.");
    logging::warn(msg);
    throw AstarteOperationRefusedException(msg);
  }
  gRPCAstarteMessage message;
  message.set_interface_name(interface_name);
  message.set_path(path);

  GrpcConverterTo converter;
  std::unique_ptr<gRPCAstarteDatastreamObject> grpc_datastream_object =
      converter(object, timestamp);
  message.set_allocated_datastream_object(grpc_datastream_object.release());

  ClientContext context;
  google::protobuf::Empty response;
  logging::trace("Sending data: {} {}", interface_name, path);
  const Status status = stub_->Send(&context, message, &response);
  if (!status.ok()) {
    logging::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    throw AstarteInvalidInputException(status.error_message());
  }
}

void AstarteDeviceGRPC::AstarteDeviceGRPCImpl::set_property(std::string_view interface_name,
                                                            std::string_view path,
                                                            const AstarteData& data) {
  logging::debug("Setting property: {} {}", interface_name, path);
  if (!connected_.load()) {
    const std::string_view msg("Device disconnected, operation aborted.");
    logging::warn(msg);
    throw AstarteOperationRefusedException(msg);
  }
  gRPCAstarteMessage message;
  message.set_interface_name(interface_name);
  message.set_path(path);

  const std::optional<AstarteData>& opt_data = data;
  GrpcConverterTo converter;
  std::unique_ptr<gRPCAstartePropertyIndividual> grpc_property_individual = converter(opt_data);
  message.set_allocated_property_individual(grpc_property_individual.release());

  ClientContext context;
  google::protobuf::Empty response;
  logging::trace("Sending data: {} {}", interface_name, path);
  const Status status = stub_->Send(&context, message, &response);
  if (!status.ok()) {
    logging::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    throw AstarteInvalidInputException(status.error_message());
  }
}

void AstarteDeviceGRPC::AstarteDeviceGRPCImpl::unset_property(std::string_view interface_name,
                                                              std::string_view path) {
  logging::debug("Unsetting property: {} {}", interface_name, path);
  if (!connected_.load()) {
    const std::string_view msg("Device disconnected, operation aborted.");
    logging::warn(msg);
    throw AstarteOperationRefusedException(msg);
  }
  gRPCAstarteMessage message;
  message.set_interface_name(interface_name);
  message.set_path(path);

  const std::optional<AstarteData> opt_data = std::nullopt;
  GrpcConverterTo converter;
  std::unique_ptr<gRPCAstartePropertyIndividual> grpc_property_individual = converter(opt_data);
  message.set_allocated_property_individual(grpc_property_individual.release());

  ClientContext context;
  google::protobuf::Empty response;
  const Status status = stub_->Send(&context, message, &response);
  if (!status.ok()) {
    logging::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    throw AstarteInvalidInputException(status.error_message());
  }
}

auto AstarteDeviceGRPC::AstarteDeviceGRPCImpl::poll_incoming(
    const std::chrono::milliseconds& timeout) -> std::optional<AstarteMessage> {
  return rcv_queue_.pop(timeout);
}

auto AstarteDeviceGRPC::AstarteDeviceGRPCImpl::get_all_properties(
    const std::optional<AstarteOwnership>& ownership) -> std::list<AstarteStoredProperty> {
  if (ownership.has_value()) {
    logging::debug("Getting all stored properties {} owned.", ownership_as_str(ownership.value()));
  } else {
    logging::debug("Getting all stored properties for all owners.");
  }

  if (!connected_.load()) {
    const std::string_view msg("Device disconnected, operation aborted.");
    logging::warn(msg);
    throw AstarteOperationRefusedException(msg);
  }

  gRPCPropertyFilter filter;
  if (ownership.has_value()) {
    filter.set_ownership((ownership == AstarteOwnership::kDevice) ? gRPCOwnership::DEVICE
                                                                  : gRPCOwnership::SERVER);
  }

  ClientContext context;
  gRPCStoredProperties response;
  const Status status = stub_->GetAllProperties(&context, filter, &response);
  if (!status.ok()) {
    logging::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    throw AstarteInvalidInputException(status.error_message());
  }

  return GrpcConverterFrom{}(response);
}

auto AstarteDeviceGRPC::AstarteDeviceGRPCImpl::get_properties(std::string_view interface_name)
    -> std::list<AstarteStoredProperty> {
  logging::debug("Getting stored properties for interface: {}", interface_name);
  if (!connected_.load()) {
    const std::string_view msg("Device disconnected, operation aborted.");
    logging::warn(msg);
    throw AstarteOperationRefusedException(msg);
  }

  gRPCInterfaceName grpc_interface_name;
  grpc_interface_name.set_name(interface_name);

  ClientContext context;
  gRPCStoredProperties response;
  const Status status = stub_->GetProperties(&context, grpc_interface_name, &response);
  if (!status.ok()) {
    logging::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    throw AstarteInvalidInputException(status.error_message());
  }

  return GrpcConverterFrom{}(response);
}

auto AstarteDeviceGRPC::AstarteDeviceGRPCImpl::get_property(std::string_view interface_name,
                                                            std::string_view path)
    -> AstartePropertyIndividual {
  logging::debug("Getting stored property for interface '{}' and path '{}'", interface_name, path);
  if (!connected_.load()) {
    const std::string_view msg("Device disconnected, operation aborted.");
    logging::warn(msg);
    throw AstarteOperationRefusedException(msg);
  }

  gRPCPropertyIdentifier identifier;
  identifier.set_interface_name(interface_name);
  identifier.set_path(path);

  ClientContext context;
  gRPCAstartePropertyIndividual response;
  const Status status = stub_->GetProperty(&context, identifier, &response);
  if (!status.ok()) {
    logging::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    throw AstarteInvalidInputException(status.error_message());
  }

  return GrpcConverterFrom{}(response);
}

// Private helper to set up the gRPC channel and stub
void AstarteDeviceGRPC::AstarteDeviceGRPCImpl::setup_grpc_channel() {
  const grpc::ChannelArguments args;
  std::vector<std::unique_ptr<ClientInterceptorFactoryInterface>> interceptor_creators;
  interceptor_creators.push_back(std::make_unique<NodeIdInterceptorFactory>(node_uuid_));

  const std::shared_ptr<Channel> channel = CreateCustomChannelWithInterceptors(
      server_addr_, grpc::InsecureChannelCredentials(), args, std::move(interceptor_creators));

  stub_ = gRPCMessageHub::NewStub(channel);
}

auto AstarteDeviceGRPC::AstarteDeviceGRPCImpl::perform_attach() -> std::optional<AttachResult> {
  // Create the node message for the attach RPC.
  gRPCNode node;
  for (const std::string& interface_json : interfaces_bins_) {
    node.add_interfaces_json(interface_json);
  }

  // Generate a new client context for the Attach method.
  // From the documentation it appears that a context needs to be valid for the duration of the RPC
  // In this case the RPC ends when the return stream is closed, so the context should survive at
  // least for that long.
  // See: https://grpc.github.io/grpc/cpp/classgrpc_1_1_client_context.html
  std::unique_ptr<ClientContext> context = std::make_unique<ClientContext>();
  std::unique_ptr<ClientReader<gRPCMessageHubEvent>> reader = stub_->Attach(context.get(), node);

  reader->WaitForInitialMetadata();
  auto server_metadata = context->GetServerInitialMetadata();
  if (server_metadata.empty()) {
    logging::warn("No metadata from server");
    grpc_stream_error_.store(true);
    return std::nullopt;
  }

  grpc_stream_error_.store(false);

  return AttachResult{.context = std::move(context), .reader = std::move(reader)};
}

void AstarteDeviceGRPC::AstarteDeviceGRPCImpl::connection_attempt(const std::stop_token& token) {
  if (connected_.load()) {
    logging::warn("Device is already connected.");
    return;
  }
  logging::debug("Attempting to connect to the message hub at {}", server_addr_);

  // Create a new channel and initialize the gRPC stub
  setup_grpc_channel();

  // perform the attach
  auto attach_res = perform_attach();
  if (!attach_res) {
    logging::error("Failed to attach to the message hub");
    return;
  }

  // the device is connected
  connected_.store(true);
  logging::info("Node connected");

  std::jthread event_handler(&AstarteDeviceGRPCImpl::handle_events, this, token,
                             std::move(attach_res->context), std::move(attach_res->reader));

  // Wait for the event stream to finish.
  if (event_handler.joinable()) {
    event_handler.join();
  }

  // the device finished its execution and is disconnected
  connected_.store(false);
  logging::info("Node disconnected");
}

void AstarteDeviceGRPC::AstarteDeviceGRPCImpl::handle_events(
    const std::stop_token& token, std::unique_ptr<grpc::ClientContext> context,
    std::unique_ptr<ClientReader<gRPCMessageHubEvent>> reader) {
  (void)context;
  logging::debug("Event handler thread has been started");

  gRPCMessageHubEvent msghub_event;
  while (!token.stop_requested() && reader->Read(&msghub_event)) {
    logging::debug("Event from the message hub received.");
    std::optional<AstarteMessage> parsed_event =
        AstarteDeviceGRPCImpl::parse_message_hub_event(msghub_event);
    if (parsed_event.has_value()) {
      const AstarteMessage& parsed_message = parsed_event.value();
      this->rcv_queue_.push(parsed_message);
    }
  }
  logging::info("Message hub stream has been interrupted.");

  // Log an error if the stream has been stopped due to a failure.
  const Status status = reader->Finish();
  if (!status.ok() && !token.stop_requested()) {
    grpc_stream_error_.store(true);
    logging::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
  }
}

auto AstarteDeviceGRPC::AstarteDeviceGRPCImpl::parse_message_hub_event(
    const gRPCMessageHubEvent& event) -> std::optional<AstarteMessage> {
  logging::trace("Parsing message hub event.");
  std::optional<AstarteMessage> res = std::nullopt;
  if (event.has_message()) {
    const gRPCAstarteMessage& astarteMessage = event.message();
    res = GrpcConverterFrom{}(astarteMessage);
  } else if (event.has_error()) {
    const gRPCMessageHubError& error = event.error();
    logging::error("Message hub error: {}", error.description());
    logging::error("Error source backtrace: ");
    for (const std::string& source : error.source()) {
      logging::error("  {}", source);
    }
  } else {
    logging::error("Unknown event type!");
  }
  return res;
}

void AstarteDeviceGRPC::AstarteDeviceGRPCImpl::connection_loop(const std::stop_token& token) {
  logging::trace("Connection loop started.");
  ExponentialBackoff backoff(std::chrono::seconds(2), std::chrono::minutes(1));

  while (!token.stop_requested()) {
    connection_attempt(token);

    if (token.stop_requested()) {
      logging::info("Stop requested, will not attempt to reconnect.");
      break;
    }

    auto delay = backoff.getNextDelay();
    logging::info("Will attempt to reconnect in {} seconds.",
                  std::chrono::duration_cast<std::chrono::seconds>(delay).count());
    std::this_thread::sleep_for(delay);
  }

  logging::info("Connection loop has been terminated.");
}

}  // namespace AstarteDeviceSdk
