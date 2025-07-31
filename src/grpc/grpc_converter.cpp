// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "grpc/grpc_converter.hpp"

#include <astarteplatform/msghub/astarte_data.pb.h>
#include <astarteplatform/msghub/astarte_message.pb.h>
#include <astarteplatform/msghub/property.pb.h>
#include <google/protobuf/timestamp.pb.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <cstdint>
#include <list>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/exceptions.hpp"
#include "astarte_device_sdk/individual.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/property.hpp"
#include "astarte_device_sdk/stored_property.hpp"
#include "grpc/grpc_formatter.hpp"  // NOLINT

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
using gRPCProperty = astarteplatform::msghub::Property;

auto GrpcConverterTo::operator()(int32_t value) -> std::unique_ptr<gRPCAstarteData> {
  spdlog::trace("Converting integer to gRPC Astarte data.");
  auto grpc_data = std::make_unique<gRPCAstarteData>();
  grpc_data->set_integer(value);
  spdlog::trace("Resulting gRPC message: \n{}", *grpc_data);
  return grpc_data;
}
auto GrpcConverterTo::operator()(int64_t value) -> std::unique_ptr<gRPCAstarteData> {
  spdlog::trace("Converting long integer to gRPC Astarte data.");
  auto grpc_data = std::make_unique<gRPCAstarteData>();
  grpc_data->set_long_integer(value);
  spdlog::trace("Resulting gRPC message: \n{}", *grpc_data);
  return grpc_data;
}
auto GrpcConverterTo::operator()(double value) -> std::unique_ptr<gRPCAstarteData> {
  spdlog::trace("Converting double to gRPC Astarte data.");
  auto grpc_data = std::make_unique<gRPCAstarteData>();
  grpc_data->set_double_(value);
  spdlog::trace("Resulting gRPC message: \n{}", *grpc_data);
  return grpc_data;
}
auto GrpcConverterTo::operator()(bool value) -> std::unique_ptr<gRPCAstarteData> {
  spdlog::trace("Converting boolean to gRPC Astarte data.");
  auto grpc_data = std::make_unique<gRPCAstarteData>();
  grpc_data->set_boolean(value);
  spdlog::trace("Resulting gRPC message: \n{}", *grpc_data);
  return grpc_data;
}
auto GrpcConverterTo::operator()(const std::string &value) -> std::unique_ptr<gRPCAstarteData> {
  spdlog::trace("Converting string to gRPC Astarte data.");
  auto grpc_data = std::make_unique<gRPCAstarteData>();
  grpc_data->set_string(value);
  spdlog::trace("Resulting gRPC message: \n{}", *grpc_data);
  return grpc_data;
}
auto GrpcConverterTo::operator()(const std::vector<uint8_t> &value)
    -> std::unique_ptr<gRPCAstarteData> {
  spdlog::trace("Converting binary blob to gRPC Astarte data.");
  auto grpc_data = std::make_unique<gRPCAstarteData>();
  std::string str_vector(value.begin(), value.end());
  grpc_data->set_binary_blob(str_vector);
  spdlog::trace("Resulting gRPC message: \n{}", *grpc_data);
  return grpc_data;
}
auto GrpcConverterTo::operator()(std::chrono::system_clock::time_point value)
    -> std::unique_ptr<gRPCAstarteData> {
  spdlog::trace("Converting date-time array to gRPC Astarte data.");
  auto grpc_data = std::make_unique<gRPCAstarteData>();
  const std::chrono::system_clock::duration t_duration = value.time_since_epoch();
  const seconds sec = duration_cast<seconds>(t_duration);
  const nanoseconds nano = duration_cast<nanoseconds>(t_duration) - sec;
  auto timestamp = std::make_unique<google::protobuf::Timestamp>();
  timestamp->set_seconds(static_cast<int64_t>(sec.count()));
  timestamp->set_nanos(static_cast<int32_t>(nano.count()));
  grpc_data->set_allocated_date_time(timestamp.release());
  spdlog::trace("Resulting gRPC message: \n{}", *grpc_data);
  return grpc_data;
}
auto GrpcConverterTo::operator()(const std::vector<int32_t> &values)
    -> std::unique_ptr<gRPCAstarteData> {
  spdlog::trace("Converting integer array to gRPC Astarte data.");
  auto grpc_data = std::make_unique<gRPCAstarteData>();
  auto grpc_array = std::make_unique<gRPCAstarteIntegerArray>();
  for (const int32_t &value : values) {
    grpc_array->add_values(value);
  }
  grpc_data->set_allocated_integer_array(grpc_array.release());
  spdlog::trace("Resulting gRPC message: \n{}", *grpc_data);
  return grpc_data;
}
auto GrpcConverterTo::operator()(const std::vector<int64_t> &values)
    -> std::unique_ptr<gRPCAstarteData> {
  spdlog::trace("Converting long integer array to gRPC Astarte data.");
  auto grpc_data = std::make_unique<gRPCAstarteData>();
  auto grpc_array = std::make_unique<gRPCAstarteLongIntegerArray>();
  for (const int64_t &value : values) {
    grpc_array->add_values(value);
  }
  grpc_data->set_allocated_long_integer_array(grpc_array.release());
  spdlog::trace("Resulting gRPC message: \n{}", *grpc_data);
  return grpc_data;
}
auto GrpcConverterTo::operator()(const std::vector<double> &values)
    -> std::unique_ptr<gRPCAstarteData> {
  spdlog::trace("Converting double array to gRPC Astarte data.");
  auto grpc_data = std::make_unique<gRPCAstarteData>();
  auto grpc_array = std::make_unique<gRPCAstarteDoubleArray>();
  for (const double &value : values) {
    grpc_array->add_values(value);
  }
  grpc_data->set_allocated_double_array(grpc_array.release());
  spdlog::trace("Resulting gRPC message: \n{}", *grpc_data);
  return grpc_data;
}
auto GrpcConverterTo::operator()(const std::vector<bool> &values)
    -> std::unique_ptr<gRPCAstarteData> {
  spdlog::trace("Converting boolean array to gRPC Astarte data.");
  auto grpc_data = std::make_unique<gRPCAstarteData>();
  auto grpc_array = std::make_unique<gRPCAstarteBooleanArray>();
  for (const bool &value : values) {
    grpc_array->add_values(value);
  }
  grpc_data->set_allocated_boolean_array(grpc_array.release());
  spdlog::trace("Resulting gRPC message: \n{}", *grpc_data);
  return grpc_data;
}
auto GrpcConverterTo::operator()(const std::vector<std::string> &values)
    -> std::unique_ptr<gRPCAstarteData> {
  spdlog::trace("Converting string array to gRPC Astarte data.");
  auto grpc_data = std::make_unique<gRPCAstarteData>();
  auto grpc_array = std::make_unique<gRPCAstarteStringArray>();
  for (const std::string &value : values) {
    grpc_array->add_values(value);
  }
  grpc_data->set_allocated_string_array(grpc_array.release());
  spdlog::trace("Resulting gRPC message: \n{}", *grpc_data);
  return grpc_data;
}
auto GrpcConverterTo::operator()(const std::vector<std::vector<uint8_t>> &values)
    -> std::unique_ptr<gRPCAstarteData> {
  spdlog::trace("Converting binary blob array to gRPC Astarte data.");
  auto grpc_data = std::make_unique<gRPCAstarteData>();
  auto grpc_array = std::make_unique<gRPCAstarteBinaryBlobArray>();
  for (const std::vector<uint8_t> &value : values) {
    const std::string str_value(value.begin(), value.end());
    grpc_array->add_values(str_value);
  }
  grpc_data->set_allocated_binary_blob_array(grpc_array.release());
  spdlog::trace("Resulting gRPC message: \n{}", *grpc_data);
  return grpc_data;
}
auto GrpcConverterTo::operator()(const std::vector<std::chrono::system_clock::time_point> &values)
    -> std::unique_ptr<gRPCAstarteData> {
  spdlog::trace("Converting date-time array to gRPC Astarte data.");
  auto grpc_data = std::make_unique<gRPCAstarteData>();
  auto grpc_array = std::make_unique<gRPCAstarteDateTimeArray>();
  for (const std::chrono::system_clock::time_point &value : values) {
    const std::chrono::system_clock::duration t_duration = value.time_since_epoch();
    const std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(t_duration);
    const std::chrono::nanoseconds nano =
        std::chrono::duration_cast<std::chrono::nanoseconds>(t_duration) - sec;
    // New timestamp in the array, allocated and managed by gRPC
    google::protobuf::Timestamp *timestamp = grpc_array->add_values();
    timestamp->set_seconds(static_cast<int64_t>(sec.count()));
    timestamp->set_nanos(static_cast<int32_t>(nano.count()));
  }
  grpc_data->set_allocated_date_time_array(grpc_array.release());
  spdlog::trace("Resulting gRPC message: \n{}", *grpc_data);
  return grpc_data;
}

// Clang-tidy assumes some of the gRPC calls are memory leaks
// NOLINTBEGIN(clang-analyzer-cplusplus.NewDeleteLeaks)
auto GrpcConverterTo::operator()(const AstarteData &value,
                                 const std::chrono::system_clock::time_point *timestamp)
    -> std::unique_ptr<gRPCAstarteDatastreamIndividual> {
  spdlog::trace("Converting Astarte datastream individual to gRPC.");
  auto grpc_individual = std::make_unique<gRPCAstarteDatastreamIndividual>();

  if (timestamp != nullptr) {
    const std::chrono::system_clock::duration t_duration = timestamp->time_since_epoch();
    const std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(t_duration);
    const std::chrono::nanoseconds nano =
        std::chrono::duration_cast<std::chrono::nanoseconds>(t_duration) - sec;
    auto grpc_timestamp = std::make_unique<google::protobuf::Timestamp>();
    grpc_timestamp->set_seconds(static_cast<int64_t>(sec.count()));
    grpc_timestamp->set_nanos(static_cast<int32_t>(nano.count()));
    grpc_individual->set_allocated_timestamp(grpc_timestamp.release());
  }

  std::unique_ptr<gRPCAstarteData> grpc_data = std::visit(GrpcConverterTo(), value.get_raw_data());
  grpc_individual->set_allocated_data(grpc_data.release());
  spdlog::trace("Resulting gRPC message: \n{}", *grpc_individual);
  return grpc_individual;
}
// NOLINTEND(clang-analyzer-cplusplus.NewDeleteLeaks)

// Clang-tidy assumes some of the gRPC calls are memory leaks
// NOLINTBEGIN(clang-analyzer-cplusplus.NewDeleteLeaks)
auto GrpcConverterTo::operator()(const AstarteDatastreamObject &value,
                                 const std::chrono::system_clock::time_point *timestamp)
    -> std::unique_ptr<gRPCAstarteDatastreamObject> {
  spdlog::trace("Converting Astarte datastream object to gRPC.");
  auto grpc_object = std::make_unique<gRPCAstarteDatastreamObject>();

  if (timestamp != nullptr) {
    const std::chrono::system_clock::duration t_duration = timestamp->time_since_epoch();
    const std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(t_duration);
    const std::chrono::nanoseconds nano =
        std::chrono::duration_cast<std::chrono::nanoseconds>(t_duration) - sec;
    auto grpc_timestamp = std::make_unique<google::protobuf::Timestamp>();
    grpc_timestamp->set_seconds(static_cast<int64_t>(sec.count()));
    grpc_timestamp->set_nanos(static_cast<int32_t>(nano.count()));
    grpc_object->set_allocated_timestamp(grpc_timestamp.release());
  }

  google::protobuf::Map<std::string, gRPCAstarteData> *grpc_map = grpc_object->mutable_data();
  for (const auto &pair : value) {
    const std::string &path = pair.first;
    const AstarteData &data = pair.second;

    const std::unique_ptr<gRPCAstarteData> grpc_data =
        std::visit(GrpcConverterTo(), data.get_raw_data());
    // NOTE: It is quite unclear from the protobuffer documentation if this assigment changes
    // ownership of the pointer. After testing with valgrind it appears that this is not the case.
    // As a consequence ownership of this grpc_data is not released.
    (*grpc_map)[path] = *grpc_data;
  }
  spdlog::trace("Resulting gRPC message: \n{}", *grpc_object);
  return grpc_object;
}
// NOLINTEND(clang-analyzer-cplusplus.NewDeleteLeaks)

auto GrpcConverterTo::operator()(const std::optional<AstarteData> &value)
    -> std::unique_ptr<gRPCAstartePropertyIndividual> {
  spdlog::trace("Converting Astarte property individual to gRPC.");
  auto grpc_property = std::make_unique<gRPCAstartePropertyIndividual>();
  if (value.has_value()) {
    const AstarteData &data = value.value();
    std::unique_ptr<gRPCAstarteData> grpc_data = std::visit(GrpcConverterTo(), data.get_raw_data());
    grpc_property->set_allocated_data(grpc_data.release());
  }
  spdlog::trace("Resulting gRPC message: \n{}", *grpc_property);
  return grpc_property;
}

// NOLINTBEGIN(readability-function-size)
auto GrpcConverterFrom::operator()(const gRPCAstarteData &value) -> AstarteData {
  spdlog::trace("Converting Astarte data from gRPC, message: \n{}", value);
  switch (value.astarte_data_case()) {
    case gRPCAstarteData::kDouble:
      spdlog::trace("Case kDouble");
      return AstarteData(value.double_());
    case gRPCAstarteData::kInteger:
      spdlog::trace("Case kInteger");
      return AstarteData(value.integer());
    case gRPCAstarteData::kBoolean:
      spdlog::trace("Case kBoolean");
      return AstarteData(value.boolean());
    case gRPCAstarteData::kLongInteger:
      spdlog::trace("Case kLongInteger");
      return AstarteData(value.long_integer());
    case gRPCAstarteData::kString:
      spdlog::trace("Case kString");
      return AstarteData(value.string());
    case gRPCAstarteData::kBinaryBlob:
      spdlog::trace("Case kBinaryBlob");
      return AstarteData(
          std::vector<uint8_t>(value.binary_blob().begin(), value.binary_blob().end()));
    case gRPCAstarteData::kDateTime: {
      spdlog::trace("Case kDateTime");
      const google::protobuf::Timestamp &timestamp = value.date_time();
      return AstarteData(std::chrono::system_clock::time_point{
          std::chrono::seconds{timestamp.seconds()} + std::chrono::nanoseconds{timestamp.nanos()}});
    }
    case gRPCAstarteData::kDoubleArray:
      spdlog::trace("Case kDoubleArray");
      return AstarteData(std::vector<double>(value.double_array().values().begin(),
                                             value.double_array().values().end()));
    case gRPCAstarteData::kIntegerArray:
      spdlog::trace("Case kIntegerArray");
      return AstarteData(std::vector<int32_t>(value.integer_array().values().begin(),
                                              value.integer_array().values().end()));
    case gRPCAstarteData::kBooleanArray:
      spdlog::trace("Case kBooleanArray");
      return AstarteData(std::vector<bool>(value.boolean_array().values().begin(),
                                           value.boolean_array().values().end()));
    case gRPCAstarteData::kLongIntegerArray:
      spdlog::trace("Case kLongIntegerArray");
      return AstarteData(std::vector<int64_t>(value.long_integer_array().values().begin(),
                                              value.long_integer_array().values().end()));
    case gRPCAstarteData::kStringArray:
      spdlog::trace("Case kStringArray");
      return AstarteData(std::vector<std::string>(value.string_array().values().begin(),
                                                  value.string_array().values().end()));
    case gRPCAstarteData::kBinaryBlobArray: {
      spdlog::trace("Case kBinaryBlobArray");
      std::vector<std::vector<uint8_t>> binblob_vect;
      for (const auto &str : value.binary_blob_array().values()) {
        binblob_vect.emplace_back(str.begin(), str.end());
      }
      return AstarteData(binblob_vect);
    }
    case gRPCAstarteData::kDateTimeArray: {
      spdlog::trace("Case kDateTimeArray");
      std::vector<std::chrono::system_clock::time_point> timestamp_vect;
      for (const auto &timestamp : value.date_time_array().values()) {
        timestamp_vect.emplace_back(std::chrono::seconds{timestamp.seconds()} +
                                    std::chrono::nanoseconds{timestamp.nanos()});
      }
      return AstarteData(timestamp_vect);
    }
    default:
      spdlog::trace("Case for gRPCAstarteData goes to default statement: ASTARTE_DATA_NOT_SET");
      break;
  }
  throw AstarteInternalException("Found an unrecognized gRPC gRPCAstarteData.");
}
// NOLINTEND(readability-function-size)

auto GrpcConverterFrom::operator()(const gRPCAstarteDatastreamIndividual &value)
    -> AstarteDatastreamIndividual {
  spdlog::trace("Converting Astarte datastream individual from gRPC, message: \n{}", value);
  const gRPCAstarteData &grpc_data(value.data());
  return AstarteDatastreamIndividual((*this)(grpc_data));
}

auto GrpcConverterFrom::operator()(const gRPCAstarteDatastreamObject &value)
    -> AstarteDatastreamObject {
  spdlog::trace("Converting Astarte datastream object from gRPC, message: \n{}", value);
  AstarteDatastreamObject object;
  const google::protobuf::Map<std::string, gRPCAstarteData> &grpc_data = value.data();
  for (const auto &[key, data] : grpc_data) {
    object.insert(key, (*this)(data));
  }
  return object;
}

auto GrpcConverterFrom::operator()(const gRPCAstartePropertyIndividual &value)
    -> AstartePropertyIndividual {
  spdlog::trace("Converting Astarte property individual from gRPC, message: \n{}", value);
  if (value.has_data()) {
    const gRPCAstarteData &grpc_data(value.data());
    return AstartePropertyIndividual((*this)(grpc_data));
  }
  return AstartePropertyIndividual(std::nullopt);
}

auto GrpcConverterFrom::operator()(const gRPCAstarteMessage &value) -> AstarteMessage {
  spdlog::trace("Converting Astarte message from gRPC, message: \n{}", value);
  if (value.has_datastream_individual()) {
    const gRPCAstarteDatastreamIndividual &grpc_datastream_individual =
        value.datastream_individual();
    const std::variant<AstarteDatastreamIndividual, AstarteDatastreamObject,
                       AstartePropertyIndividual>
        parsed_data((*this)(grpc_datastream_individual));
    return {value.interface_name(), value.path(), parsed_data};
  }
  if (value.has_datastream_object()) {
    const gRPCAstarteDatastreamObject &grpc_datastream_object = value.datastream_object();
    const std::variant<AstarteDatastreamIndividual, AstarteDatastreamObject,
                       AstartePropertyIndividual>
        parsed_data((*this)(grpc_datastream_object));
    return {value.interface_name(), value.path(), parsed_data};
  }
  if (value.has_property_individual()) {
    const gRPCAstartePropertyIndividual &grpc_property_individual = value.property_individual();
    const std::variant<AstarteDatastreamIndividual, AstarteDatastreamObject,
                       AstartePropertyIndividual>
        parsed_data((*this)(grpc_property_individual));
    return {value.interface_name(), value.path(), parsed_data};
  }
  throw AstarteInternalException("Found an unrecognized gRPC gRPCAstarteDataType.");
}

auto GrpcConverterFrom::operator()(const gRPCOwnership &value) -> AstarteOwnership {
  spdlog::trace("Converting Astarte ownership from gRPC.");
  return (value == gRPCOwnership::DEVICE) ? AstarteOwnership::kDevice : AstarteOwnership::kServer;
}

auto GrpcConverterFrom::operator()(const gRPCStoredProperties &value)
    -> std::list<AstarteStoredProperty> {
  spdlog::trace("Converting Astarte stored property from gRPC.");
  std::list<AstarteStoredProperty> stored_properties;
  for (const gRPCProperty &stored_property : value.properties()) {
    stored_properties.emplace_back(
        stored_property.interface_name(), stored_property.path(), stored_property.version_major(),
        (*this)(stored_property.ownership()), (*this)(stored_property.data()));
  }
  return stored_properties;
}

}  // namespace AstarteDeviceSdk
