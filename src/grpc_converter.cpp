// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "grpc_converter.hpp"

#include <astarteplatform/msghub/astarte_data.pb.h>
#include <astarteplatform/msghub/astarte_message.pb.h>
#include <google/protobuf/timestamp.pb.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/exceptions.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"

namespace AstarteDeviceSdk {

using std::chrono::duration_cast;
using std::chrono::nanoseconds;
using std::chrono::seconds;

using gRPCAstarteBinaryBlobArray = astarteplatform::msghub::AstarteBinaryBlobArray;
using gRPCAstarteBooleanArray = astarteplatform::msghub::AstarteBooleanArray;
using gRPCAstarteDateTimeArray = astarteplatform::msghub::AstarteDateTimeArray;
using gRPCAstarteDoubleArray = astarteplatform::msghub::AstarteDoubleArray;
using gRPCAstarteIntegerArray = astarteplatform::msghub::AstarteIntegerArray;
using gRPCAstarteLongIntegerArray = astarteplatform::msghub::AstarteLongIntegerArray;
using gRPCAstarteStringArray = astarteplatform::msghub::AstarteStringArray;
using gRPCAstarteData = astarteplatform::msghub::AstarteData;
using gRPCAstarteDatastreamIndividual = astarteplatform::msghub::AstarteDatastreamIndividual;
using gRPCAstarteDatastreamObject = astarteplatform::msghub::AstarteDatastreamObject;
using gRPCAstartePropertyIndividual = astarteplatform::msghub::AstartePropertyIndividual;
using gRPCAstarteMessage = astarteplatform::msghub::AstarteMessage;

auto GrpcConverter::operator()(int32_t value) -> gRPCAstarteData * {
  auto *grpc_data = new gRPCAstarteData();
  grpc_data->set_integer(value);
  spdlog::trace("AstarteData converted to gRPC: {}", grpc_data->DebugString());
  return grpc_data;
}
auto GrpcConverter::operator()(int64_t value) -> gRPCAstarteData * {
  auto *grpc_data = new gRPCAstarteData();
  grpc_data->set_long_integer(value);
  spdlog::trace("AstarteData converted to gRPC: {}", grpc_data->DebugString());
  return grpc_data;
}
auto GrpcConverter::operator()(double value) -> gRPCAstarteData * {
  auto *grpc_data = new gRPCAstarteData();
  grpc_data->set_double_(value);
  spdlog::trace("AstarteData converted to gRPC: {}", grpc_data->DebugString());
  return grpc_data;
}
auto GrpcConverter::operator()(bool value) -> gRPCAstarteData * {
  auto *grpc_data = new gRPCAstarteData();
  grpc_data->set_boolean(value);
  spdlog::trace("AstarteData converted to gRPC: {}", grpc_data->DebugString());
  return grpc_data;
}
auto GrpcConverter::operator()(const std::string &value) -> gRPCAstarteData * {
  auto *grpc_data = new gRPCAstarteData();
  grpc_data->set_string(value);
  spdlog::trace("AstarteData converted to gRPC: {}", grpc_data->DebugString());
  return grpc_data;
}
auto GrpcConverter::operator()(const std::vector<uint8_t> &value) -> gRPCAstarteData * {
  auto *grpc_data = new gRPCAstarteData();
  std::string str_vector(value.begin(), value.end());
  grpc_data->set_binary_blob(str_vector);
  spdlog::trace("AstarteData converted to gRPC: {}", grpc_data->DebugString());
  return grpc_data;
}
auto GrpcConverter::operator()(std::chrono::system_clock::time_point value) -> gRPCAstarteData * {
  auto *grpc_data = new gRPCAstarteData();
  const std::chrono::system_clock::duration t_duration = value.time_since_epoch();
  const seconds sec = duration_cast<seconds>(t_duration);
  const nanoseconds nano = duration_cast<nanoseconds>(t_duration) - sec;
  auto *timestamp = new google::protobuf::Timestamp();
  timestamp->set_seconds(static_cast<int64_t>(sec.count()));
  timestamp->set_nanos(static_cast<int32_t>(nano.count()));
  grpc_data->set_allocated_date_time(timestamp);
  spdlog::trace("AstarteData converted to gRPC: {}", grpc_data->DebugString());
  return grpc_data;
}
auto GrpcConverter::operator()(const std::vector<int32_t> &values) -> gRPCAstarteData * {
  auto *grpc_data = new gRPCAstarteData();
  auto *grpc_array = new gRPCAstarteIntegerArray();
  for (const int32_t &value : values) {
    grpc_array->add_values(value);
  }
  grpc_data->set_allocated_integer_array(grpc_array);
  spdlog::trace("AstarteData converted to gRPC: {}", grpc_data->DebugString());
  return grpc_data;
}
auto GrpcConverter::operator()(const std::vector<int64_t> &values) -> gRPCAstarteData * {
  auto *grpc_data = new gRPCAstarteData();
  auto *grpc_array = new gRPCAstarteLongIntegerArray();
  for (const int64_t &value : values) {
    grpc_array->add_values(value);
  }
  grpc_data->set_allocated_long_integer_array(grpc_array);
  spdlog::trace("AstarteData converted to gRPC: {}", grpc_data->DebugString());
  return grpc_data;
}
auto GrpcConverter::operator()(const std::vector<double> &values) -> gRPCAstarteData * {
  auto *grpc_data = new gRPCAstarteData();
  auto *grpc_array = new gRPCAstarteDoubleArray();
  for (const double &value : values) {
    grpc_array->add_values(value);
  }
  grpc_data->set_allocated_double_array(grpc_array);
  spdlog::trace("AstarteData converted to gRPC: {}", grpc_data->DebugString());
  return grpc_data;
}
auto GrpcConverter::operator()(const std::vector<bool> &values) -> gRPCAstarteData * {
  auto *grpc_data = new gRPCAstarteData();
  auto *grpc_array = new gRPCAstarteBooleanArray();
  for (const bool &value : values) {
    grpc_array->add_values(value);
  }
  grpc_data->set_allocated_boolean_array(grpc_array);
  spdlog::trace("AstarteData converted to gRPC: {}", grpc_data->DebugString());
  return grpc_data;
}
auto GrpcConverter::operator()(const std::vector<std::string> &values) -> gRPCAstarteData * {
  auto *grpc_data = new gRPCAstarteData();
  auto *grpc_array = new gRPCAstarteStringArray();
  for (const std::string &value : values) {
    grpc_array->add_values(value);
  }
  grpc_data->set_allocated_string_array(grpc_array);
  spdlog::trace("AstarteData converted to gRPC: {}", grpc_data->DebugString());
  return grpc_data;
}
auto GrpcConverter::operator()(const std::vector<std::vector<uint8_t>> &values)
    -> gRPCAstarteData * {
  auto *grpc_data = new gRPCAstarteData();
  auto *grpc_array = new gRPCAstarteBinaryBlobArray();
  for (const std::vector<uint8_t> &value : values) {
    const std::string str_value(value.begin(), value.end());
    grpc_array->add_values(str_value);
  }
  grpc_data->set_allocated_binary_blob_array(grpc_array);
  spdlog::trace("AstarteData converted to gRPC: {}", grpc_data->DebugString());
  return grpc_data;
}
auto GrpcConverter::operator()(const std::vector<std::chrono::system_clock::time_point> &values)
    -> gRPCAstarteData * {
  auto *grpc_data = new gRPCAstarteData();
  auto *grpc_array = new gRPCAstarteDateTimeArray();
  for (const std::chrono::system_clock::time_point &value : values) {
    const std::chrono::system_clock::duration t_duration = value.time_since_epoch();
    const std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(t_duration);
    const std::chrono::nanoseconds nano =
        std::chrono::duration_cast<std::chrono::nanoseconds>(t_duration) - sec;
    // Instantiate the protobuf timestamp
    google::protobuf::Timestamp *timestamp = grpc_array->add_values();
    timestamp->set_seconds(static_cast<int64_t>(sec.count()));
    timestamp->set_nanos(static_cast<int32_t>(nano.count()));
  }
  grpc_data->set_allocated_date_time_array(grpc_array);
  spdlog::trace("AstarteData converted to gRPC: {}", grpc_data->DebugString());
  return grpc_data;
}

auto GrpcConverter::operator()(const AstarteData &value,
                               const std::chrono::system_clock::time_point *timestamp)
    -> gRPCAstarteDatastreamIndividual * {
  auto *grpc_individual = new gRPCAstarteDatastreamIndividual();

  google::protobuf::Timestamp *grpc_timestamp = nullptr;
  if (timestamp != nullptr) {
    const std::chrono::system_clock::duration t_duration = timestamp->time_since_epoch();
    const std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(t_duration);
    const std::chrono::nanoseconds nano =
        std::chrono::duration_cast<std::chrono::nanoseconds>(t_duration) - sec;
    grpc_timestamp = new google::protobuf::Timestamp();
    grpc_timestamp->set_seconds(static_cast<int64_t>(sec.count()));
    grpc_timestamp->set_nanos(static_cast<int32_t>(nano.count()));
    grpc_individual->set_allocated_timestamp(grpc_timestamp);
  }

  gRPCAstarteData *grpc_data = std::visit(GrpcConverter(), value.get_raw_data());
  grpc_individual->set_allocated_data(grpc_data);
  return grpc_individual;
}

auto GrpcConverter::operator()(const AstarteObject &value,
                               const std::chrono::system_clock::time_point *timestamp)
    -> gRPCAstarteDatastreamObject * {
  auto *grpc_object = new gRPCAstarteDatastreamObject();

  google::protobuf::Timestamp *grpc_timestamp = nullptr;
  if (timestamp != nullptr) {
    const std::chrono::system_clock::duration t_duration = timestamp->time_since_epoch();
    const std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(t_duration);
    const std::chrono::nanoseconds nano =
        std::chrono::duration_cast<std::chrono::nanoseconds>(t_duration) - sec;
    grpc_timestamp = new google::protobuf::Timestamp();
    grpc_timestamp->set_seconds(static_cast<int64_t>(sec.count()));
    grpc_timestamp->set_nanos(static_cast<int32_t>(nano.count()));
    grpc_object->set_allocated_timestamp(grpc_timestamp);
  }

  google::protobuf::Map<std::string, gRPCAstarteData> *grpc_map = grpc_object->mutable_data();
  for (const auto &pair : value) {
    const std::string &path = pair.first;
    const AstarteData &data = pair.second;
    gRPCAstarteData *grpc_data = std::visit(GrpcConverter(), data.get_raw_data());
    // TODO(simone): This could be a memory leak. It should be investigated.
    (*grpc_map)[path] = *grpc_data;
  }
  return grpc_object;
}

auto GrpcConverter::operator()(const std::optional<AstarteData> &value)
    -> gRPCAstartePropertyIndividual * {
  auto *grpc_property = new gRPCAstartePropertyIndividual();
  if (value.has_value()) {
    const AstarteData &data = value.value();
    gRPCAstarteData *grpc_data = std::visit(GrpcConverter(), data.get_raw_data());
    grpc_property->set_allocated_data(grpc_data);
  }
  // Check if has_data is false when optional has no value (if it's true use clear_data)
  return grpc_property;
}

// NOLINTBEGIN(readability-function-size)
auto GrpcConverter::operator()(const gRPCAstarteData &value) -> AstarteData {
  switch (value.astarte_data_case()) {
    case gRPCAstarteData::kDouble:
      return AstarteData(value.double_());
    case gRPCAstarteData::kInteger:
      return AstarteData(value.integer());
    case gRPCAstarteData::kBoolean:
      return AstarteData(value.boolean());
    case gRPCAstarteData::kLongInteger:
      return AstarteData(value.long_integer());
    case gRPCAstarteData::kString:
      return AstarteData(value.string());
    case gRPCAstarteData::kBinaryBlob:
      return AstarteData(
          std::vector<uint8_t>(value.binary_blob().begin(), value.binary_blob().end()));
    case gRPCAstarteData::kDateTime: {
      const google::protobuf::Timestamp &timestamp = value.date_time();
      return AstarteData(std::chrono::system_clock::time_point{
          std::chrono::seconds{timestamp.seconds()} + std::chrono::nanoseconds{timestamp.nanos()}});
    }
    case gRPCAstarteData::kDoubleArray:
      return AstarteData(std::vector<double>(value.double_array().values().begin(),
                                             value.double_array().values().end()));
    case gRPCAstarteData::kIntegerArray:
      return AstarteData(std::vector<int32_t>(value.integer_array().values().begin(),
                                              value.integer_array().values().end()));
    case gRPCAstarteData::kBooleanArray:
      return AstarteData(std::vector<bool>(value.boolean_array().values().begin(),
                                           value.boolean_array().values().end()));
    case gRPCAstarteData::kLongIntegerArray:
      return AstarteData(std::vector<int64_t>(value.long_integer_array().values().begin(),
                                              value.long_integer_array().values().end()));
    case gRPCAstarteData::kStringArray:
      return AstarteData(std::vector<std::string>(value.string_array().values().begin(),
                                                  value.string_array().values().end()));
    case gRPCAstarteData::kBinaryBlobArray: {
      std::vector<std::vector<uint8_t>> binblob_vect;
      for (const auto &str : value.binary_blob_array().values()) {
        binblob_vect.emplace_back(str.begin(), str.end());
      }
      return AstarteData(binblob_vect);
    }
    case gRPCAstarteData::kDateTimeArray: {
      std::vector<std::chrono::system_clock::time_point> timestamp_vect;
      for (const auto &timestamp : value.date_time_array().values()) {
        timestamp_vect.emplace_back(std::chrono::seconds{timestamp.seconds()} +
                                    std::chrono::nanoseconds{timestamp.nanos()});
      }
      return AstarteData(timestamp_vect);
    }
    default:
      break;
  }
  throw AstarteInternalException("Found an unrecognized gRPC gRPCAstarteData.");
}
// NOLINTEND(readability-function-size)

auto GrpcConverter::operator()(const gRPCAstarteDatastreamObject &value) -> AstarteObject {
  AstarteObject object;
  const google::protobuf::Map<std::string, gRPCAstarteData> &grpc_data = value.data();
  GrpcConverter converter;
  for (const auto &[key, data] : grpc_data) {
    object.insert(key, converter(data));
  }
  return object;
}

auto GrpcConverter::operator()(const gRPCAstarteMessage &value) -> AstarteMessage {
  std::optional<std::variant<AstarteData, AstarteObject>> parsed_data;
  if (value.has_datastream_individual()) {
    const gRPCAstarteDatastreamIndividual &grpc_datastream_individual =
        value.datastream_individual();
    const gRPCAstarteData &grpc_data = grpc_datastream_individual.data();
    GrpcConverter converter;
    parsed_data = converter(grpc_data);
  } else if (value.has_datastream_object()) {
    const gRPCAstarteDatastreamObject &grpc_datastream_object = value.datastream_object();
    GrpcConverter converter;
    parsed_data = converter(grpc_datastream_object);
  } else if (value.has_property_individual()) {
    const gRPCAstartePropertyIndividual &grpc_property_individual = value.property_individual();
    if (grpc_property_individual.has_data()) {
      const gRPCAstarteData &grpc_data = grpc_property_individual.data();
      GrpcConverter converter;
      parsed_data = converter(grpc_data);
    } else {
      parsed_data = std::nullopt;
    }

    // } else if (data.has_astarte_object()) {
    //     GrpcConverter converter;
    //     parsed_data = converter(data.astarte_object());
    //   } else {
    //
    //   }

  } else {
    throw AstarteInternalException("Found an unrecognized gRPC gRPCAstarteDataType.");
  }
  return {value.interface_name(), value.path(), parsed_data};
}

}  // namespace AstarteDeviceSdk
