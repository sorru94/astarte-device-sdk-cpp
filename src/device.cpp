// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/device.h"

#include <astarteplatform/msghub/astarte_message.pb.h>
#include <astarteplatform/msghub/astarte_type.pb.h>
#include <astarteplatform/msghub/message_hub_error.pb.h>
#include <astarteplatform/msghub/message_hub_service.grpc.pb.h>
#include <astarteplatform/msghub/node.pb.h>
#include <google/protobuf/empty.pb.h>
#include <google/protobuf/timestamp.pb.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/support/channel_arguments.h>
#include <grpcpp/support/client_interceptor.h>
#include <grpcpp/support/status.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <cstdint>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "astarte_device_sdk/exceptions.h"
#include "astarte_device_sdk/individual.h"
#include "grpc_interceptors.h"
#include "individual_private.h"
#include "object_private.h"
#include "shared_queue.h"

namespace AstarteDeviceSdk {

using ParsedAstarteDataType =
    std::variant<AstarteIndividual, std::unordered_map<std::string, AstarteIndividual>>;
using ParsedAstarteMessage =
    std::tuple<std::string, std::string, std::optional<ParsedAstarteDataType>>;
using ParsedAstarteMessageHubEvent = std::optional<ParsedAstarteMessage>;

using std::make_unique;
using std::shared_ptr;

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::Status;

using grpc::experimental::ClientInterceptorFactoryInterface;

using astarteplatform::msghub::AstarteDataType;
using astarteplatform::msghub::AstarteDataTypeIndividual;
using astarteplatform::msghub::AstarteMessage;
using astarteplatform::msghub::AstarteUnset;
using astarteplatform::msghub::MessageHub;
using astarteplatform::msghub::MessageHubError;
using astarteplatform::msghub::MessageHubEvent;
using astarteplatform::msghub::Node;

// This class is only used for the PImpl of the class AstarteDeviceAstarteDevice, as such member
// variables are not required to be private.
// NOLINTBEGIN(misc-non-private-member-variables-in-classes)
struct AstarteDevice::AstarteDeviceImpl {
 public:
  AstarteDeviceImpl(std::string server_addr, std::string node_uuid)
      : server_addr_(std::move(server_addr)), node_uuid_(std::move(node_uuid)) {}

  /** @brief Handle the Astarte message hub events stream. */
  void handle_events(std::unique_ptr<ClientReader<MessageHubEvent>> reader) {
    spdlog::debug("Event handler thread has been started");

    // Read the message stream.
    MessageHubEvent msghub_event;
    while (reader->Read(&msghub_event)) {
      spdlog::debug("Event from the message hub received.");
      ParsedAstarteMessageHubEvent parsed_event =
          AstarteDeviceImpl::parse_message_hub_event(msghub_event);
      if (parsed_event.has_value()) {
        const ParsedAstarteMessage &parsed_message = parsed_event.value();
        this->rcv_queue_.push(parsed_message);
      }
    }
    spdlog::info("Message hub stream has been interrupted.");

    // Log an error if it the stream has been stopped due to a failure.
    const Status status = reader->Finish();
    if (!status.ok()) {
      spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    }
  }
  /** @brief gRPC server address for the Astarte message hub. */
  std::string server_addr_;
  /** @brief Unique identifier for the device connection with the Astarte message hub. */
  std::string node_uuid_;
  /** @brief Stub for the gRPC message hub service. */
  std::unique_ptr<MessageHub::Stub> stub_;
  /** @brief List of json interfaces. Stored as binaries. */
  std::vector<std::string> interfaces_bins_;
  /** @brief Thread that handles the message hub events stream. */
  std::thread event_handler_;
  /**
   * @brief gRPC context for the client, unused.
   * @details It could be used to convey extra information to the server and/or tweak certain
   RPC
   * behaviors.
   */
  ClientContext client_context_;
  /** @brief Receive queue for the Astarte device. */
  SharedQueue<ParsedAstarteMessage> rcv_queue_;

 private:
  /** @brief Parse a single message hub event. */
  static auto parse_message_hub_event(const MessageHubEvent &event)
      -> ParsedAstarteMessageHubEvent {
    ParsedAstarteMessageHubEvent res = std::nullopt;
    if (event.has_message()) {
      const AstarteMessage &astarteMessage = event.message();
      res = parse_astarte_message(astarteMessage);
    } else if (event.has_error()) {
      const MessageHubError &error = event.error();
      spdlog::error("Message hub error: {}", error.description());
      spdlog::error("Error source backtrace: ");
      for (const std::string &source : error.source()) {
        spdlog::error("  {}", source);
      }
    } else {
      spdlog::error("Unknown event type!");
    }
    return res;
  }
  /** @brief Parse an Astarte message. */
  static auto parse_astarte_message(const AstarteMessage &message) -> ParsedAstarteMessage {
    std::optional<ParsedAstarteDataType> value;
    if (message.has_astarte_data()) {
      const AstarteDataType &data = message.astarte_data();
      value = parse_astarte_data_type(data);
    } else if (message.has_astarte_unset()) {
      spdlog::debug("Received unset message.");
      value = std::nullopt;
    }
    return {message.interface_name(), message.path(), value};
  }
  /** @brief Parse an Astarte data type. */
  static auto parse_astarte_data_type(const AstarteDataType &data) -> ParsedAstarteDataType {
    ParsedAstarteDataType res;
    if (data.has_astarte_individual()) {
      const AstarteDataTypeIndividual &individual = data.astarte_individual();
      spdlog::debug("Received individual message from Astarte: \n{}", individual.DebugString());
      res = parse_astarte_data_type_individual(individual);
    } else if (data.has_astarte_object()) {
      std::unordered_map<std::string, AstarteIndividual> parsed_object_data;
      const google::protobuf::Map<std::string, AstarteDataTypeIndividual> &object_data =
          data.astarte_object().object_data();
      spdlog::debug("Received object message from Astarte");
      for (const auto &[key, individual] : object_data) {
        spdlog::debug("[{}]: {}", key, individual.DebugString());
        parsed_object_data[key] = parse_astarte_data_type_individual(individual);
      }
      res = parsed_object_data;
    }
    return res;
  }
  /** @brief Parse an Astarte data type individual. */
  static auto parse_astarte_data_type_individual(const AstarteDataTypeIndividual &individual)
      -> AstarteIndividual {
    AstarteIndividual parsed_individual;
    switch (individual.individual_data_case()) {
      case AstarteDataTypeIndividual::kAstarteDouble:
        parsed_individual = individual.astarte_double();
        break;
      case AstarteDataTypeIndividual::kAstarteInteger:
        parsed_individual = individual.astarte_integer();
        break;
      case AstarteDataTypeIndividual::kAstarteBoolean:
        parsed_individual = individual.astarte_boolean();
        break;
      case AstarteDataTypeIndividual::kAstarteLongInteger:
        parsed_individual = individual.astarte_long_integer();
        break;
      case AstarteDataTypeIndividual::kAstarteString:
        parsed_individual = individual.astarte_string();
        break;
      case AstarteDataTypeIndividual::kAstarteBinaryBlob:
        parsed_individual = std::vector<uint8_t>(individual.astarte_binary_blob().begin(),
                                                 individual.astarte_binary_blob().end());
        break;
      case AstarteDataTypeIndividual::kAstarteDateTime: {
        const google::protobuf::Timestamp &timestamp = individual.astarte_date_time();
        parsed_individual =
            std::chrono::system_clock::time_point{std::chrono::seconds{timestamp.seconds()} +
                                                  std::chrono::nanoseconds{timestamp.nanos()}};
        break;
      }
      case AstarteDataTypeIndividual::kAstarteDoubleArray:
        parsed_individual = std::vector<double>(individual.astarte_double_array().values().begin(),
                                                individual.astarte_double_array().values().end());
        break;
      case AstarteDataTypeIndividual::kAstarteIntegerArray:
        parsed_individual =
            std::vector<int32_t>(individual.astarte_integer_array().values().begin(),
                                 individual.astarte_integer_array().values().end());
        break;
      case AstarteDataTypeIndividual::kAstarteBooleanArray:
        parsed_individual = std::vector<bool>(individual.astarte_boolean_array().values().begin(),
                                              individual.astarte_boolean_array().values().end());
        break;
      case AstarteDataTypeIndividual::kAstarteLongIntegerArray:
        parsed_individual =
            std::vector<int64_t>(individual.astarte_long_integer_array().values().begin(),
                                 individual.astarte_long_integer_array().values().end());
        break;
      case AstarteDataTypeIndividual::kAstarteStringArray:
        parsed_individual =
            std::vector<std::string>(individual.astarte_string_array().values().begin(),
                                     individual.astarte_string_array().values().end());
        break;
      case AstarteDataTypeIndividual::kAstarteBinaryBlobArray: {
        std::vector<std::vector<uint8_t>> binblob_vect;
        for (const auto &str : individual.astarte_binary_blob_array().values()) {
          binblob_vect.emplace_back(str.begin(), str.end());
        }
        parsed_individual = binblob_vect;
        break;
      }
      case AstarteDataTypeIndividual::kAstarteDateTimeArray: {
        std::vector<std::chrono::system_clock::time_point> timestamp_vect;
        for (const auto &timestamp : individual.astarte_date_time_array().values()) {
          timestamp_vect.emplace_back(std::chrono::seconds{timestamp.seconds()} +
                                      std::chrono::nanoseconds{timestamp.nanos()});
        }
        parsed_individual = timestamp_vect;
        break;
      }
      default:
        break;
    }
    return parsed_individual;
  }
};

// NOLINTEND(misc-non-private-member-variables-in-classes)

AstarteDevice::AstarteDevice(const std::string &server_addr, const std::string &node_uuid)
    : astarte_device_impl_{std::make_shared<AstarteDeviceImpl>(server_addr, node_uuid)} {}

AstarteDevice::~AstarteDevice() = default;

void AstarteDevice::add_interface_from_json(const std::filesystem::path &json_file) {
  spdlog::debug("Adding interface from file: {}", json_file.string());
  std::ifstream interface_file(json_file, std::ios::in);
  if (!interface_file.is_open()) {
    spdlog::error("Could not open the interface file: {}", json_file.string());
    throw AstarteFileOpenException(json_file.string());
  }
  // Read the entire JSON file content into a string
  const std::string interface_json((std::istreambuf_iterator<char>(interface_file)),
                                   std::istreambuf_iterator<char>());
  astarte_device_impl_->interfaces_bins_.push_back(interface_json);
  spdlog::trace("Added interface: \n{}", interface_json);
  interface_file.close();
}

void AstarteDevice::connect() {
  spdlog::info("Connecting to the message hub");

  // Create a new channel and initialize the gRPC stub
  const grpc::ChannelArguments args;
  std::vector<std::unique_ptr<ClientInterceptorFactoryInterface>> interceptor_creators;
  interceptor_creators.push_back(
      make_unique<NodeIdInterceptorFactory>(astarte_device_impl_->node_uuid_));

  const shared_ptr<Channel> channel = CreateCustomChannelWithInterceptors(
      astarte_device_impl_->server_addr_, grpc::InsecureChannelCredentials(), args,
      std::move(interceptor_creators));

  astarte_device_impl_->stub_ = MessageHub::NewStub(channel);

  // Create the node message for the attach RPC.
  Node node;
  for (const std::string &interface_json : astarte_device_impl_->interfaces_bins_) {
    node.add_interfaces_json(interface_json);
  }

  // Call the attach RPC.
  std::unique_ptr<ClientReader<MessageHubEvent>> reader =
      astarte_device_impl_->stub_->Attach(&astarte_device_impl_->client_context_, node);

  // Start a thread for the event stream
  astarte_device_impl_->event_handler_ =
      std::thread(&AstarteDevice::AstarteDeviceImpl::handle_events, this->astarte_device_impl_,
                  std::move(reader));
}

void AstarteDevice::disconnect() {
  spdlog::info("Disconnecting from the message hub");

  // Call the dettach RPC.
  ClientContext context;
  google::protobuf::Empty response;
  const Status status =
      astarte_device_impl_->stub_->Detach(&context, google::protobuf::Empty(), &response);
  if (!status.ok()) {
    spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
  }
}

void AstarteDevice::send_individual(const std::string &interface_name, const std::string &path,
                                    AstarteIndividual &individual,
                                    std::chrono::system_clock::time_point *timestamp) {
  auto *grpc_data = new AstarteDataType();
  AstarteDataTypeIndividual *grpc_individual =
      std::visit(AstarteIndividualToAstarteDataTypeIndividual(), individual);

  grpc_data->set_allocated_astarte_individual(grpc_individual);

  google::protobuf::Timestamp *grpc_timestamp = nullptr;
  if (timestamp != nullptr) {
    const std::chrono::system_clock::duration t_duration = timestamp->time_since_epoch();
    const std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(t_duration);
    const std::chrono::nanoseconds nano =
        std::chrono::duration_cast<std::chrono::nanoseconds>(t_duration) - sec;
    grpc_timestamp = new google::protobuf::Timestamp();
    grpc_timestamp->set_seconds(static_cast<int64_t>(sec.count()));
    grpc_timestamp->set_nanos(static_cast<int32_t>(nano.count()));
  }

  AstarteMessage message;
  message.set_interface_name(interface_name);
  message.set_path(path);
  message.set_allocated_astarte_data(grpc_data);
  if (timestamp != nullptr) {
    message.set_allocated_timestamp(grpc_timestamp);
  }

  ClientContext context;
  google::protobuf::Empty response;
  spdlog::debug("Sending individual data: {} {}", interface_name, path);
  spdlog::trace("Content: \n{}", message.DebugString());
  const Status status = astarte_device_impl_->stub_->Send(&context, message, &response);
  if (!status.ok()) {
    spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    throw AstarteInvalidInputException(status.error_message());
  }
}

void AstarteDevice::send_object(const std::string &interface_name, const std::string &path,
                                std::unordered_map<std::string, AstarteIndividual> &object,
                                std::chrono::system_clock::time_point *timestamp) {
  auto *grpc_data = new AstarteDataType();
  AstarteObjectToAstarteDataTypeObject converter;
  AstarteDataTypeObject *grpc_object = converter(object);
  grpc_data->set_allocated_astarte_object(grpc_object);

  google::protobuf::Timestamp *grpc_timestamp = nullptr;
  if (timestamp != nullptr) {
    const std::chrono::system_clock::duration t_duration = timestamp->time_since_epoch();
    const std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(t_duration);
    const std::chrono::nanoseconds nano =
        std::chrono::duration_cast<std::chrono::nanoseconds>(t_duration) - sec;
    grpc_timestamp = new google::protobuf::Timestamp();
    grpc_timestamp->set_seconds(static_cast<int64_t>(sec.count()));
    grpc_timestamp->set_nanos(static_cast<int32_t>(nano.count()));
  }

  AstarteMessage message;
  message.set_interface_name(interface_name);
  message.set_path(path);
  // The deallocation of grpc_data and grpc_timestamp is handled by gRPC
  // NOLINTBEGIN(clang-analyzer-cplusplus.NewDeleteLeaks)
  message.set_allocated_astarte_data(grpc_data);
  if (timestamp != nullptr) {
    message.set_allocated_timestamp(grpc_timestamp);
  }

  ClientContext context;
  google::protobuf::Empty response;
  spdlog::debug("Sending object data: {} {}", interface_name, path);
  spdlog::trace("Content: \n{}", message.DebugString());
  const Status status = astarte_device_impl_->stub_->Send(&context, message, &response);
  if (!status.ok()) {
    spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    throw AstarteInvalidInputException(status.error_message());
  }
  // NOLINTEND(clang-analyzer-cplusplus.NewDeleteLeaks)
}

void AstarteDevice::set_property(const std::string &interface_name, const std::string &path,
                                 AstarteIndividual &data) {
  this->send_individual(interface_name, path, data, nullptr);
}

void AstarteDevice::unset_property(const std::string &interface_name, const std::string &path) {
  auto *grpc_data = new AstarteUnset();
  AstarteMessage message;
  message.set_interface_name(interface_name);
  message.set_path(path);
  message.set_allocated_astarte_unset(grpc_data);

  ClientContext context;
  google::protobuf::Empty response;
  spdlog::debug("Unsetting property: {} {}", interface_name, path);
  const Status status = astarte_device_impl_->stub_->Send(&context, message, &response);
  if (!status.ok()) {
    spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    throw AstarteInvalidInputException(status.error_message());
  }
}

auto AstarteDevice::poll_incoming() -> std::optional<
    std::tuple<std::string, std::string,
               std::optional<std::variant<AstarteIndividual,
                                          std::unordered_map<std::string, AstarteIndividual>>>>> {
  return astarte_device_impl_->rcv_queue_.pop();
}

}  // namespace AstarteDeviceSdk
