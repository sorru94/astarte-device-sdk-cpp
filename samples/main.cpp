// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <spdlog/spdlog.h>

#include <iostream>
#include <thread>

#include "astarte_device_sdk/device.hpp"
#include "astarte_device_sdk/individual.hpp"
#include "astarte_device_sdk/msg.hpp"

using AstarteDeviceSdk::AstarteDevice;
using AstarteDeviceSdk::AstarteIndividual;
using AstarteDeviceSdk::AstarteMessage;
using AstarteDeviceSdk::AstarteObject;

void reception_handler(std::shared_ptr<AstarteDevice> msghub_client) {
  while (true) {
    auto incoming = msghub_client->poll_incoming();
    if (incoming.has_value()) {
      AstarteMessage msg(incoming.value());
      spdlog::info("Received message: {}", msg.format());
      if (msg.get_interface().find("org.astarte-platform.cpp.examples.ServerDatastream") !=
              std::string::npos &&
          msg.get_path().find("double_endpoint") != std::string::npos && msg.into().has_value() &&
          std::holds_alternative<AstarteIndividual>(msg.into().value())) {
        const AstarteIndividual &data = std::get<AstarteIndividual>(msg.into().value());
        double value = data.into<double>();
        // Use the received value
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

int main(int argc, char **argv) {
  spdlog::set_level(spdlog::level::debug);
  std::string server_addr = "localhost:50051";
  std::string node_id("aa04dade-9401-4c37-8c6a-d8da15b083ae");
  std::shared_ptr<AstarteDevice> msghub_client =
      std::make_shared<AstarteDevice>(server_addr, node_id);

  // Those paths assume the user is calling the Astarte executable from the root of this project.
  std::filesystem::path device_individual_interface_file_path =
      "samples/interfaces/org.astarte-platform.cpp.examples.DeviceDatastream.json";
  msghub_client->add_interface_from_json(device_individual_interface_file_path);
  std::filesystem::path server_individual_interface_file_path =
      "samples/interfaces/org.astarte-platform.cpp.examples.ServerDatastream.json";
  msghub_client->add_interface_from_json(server_individual_interface_file_path);
  std::filesystem::path device_property_interface_file_path =
      "samples/interfaces/org.astarte-platform.cpp.examples.DeviceProperty.json";
  msghub_client->add_interface_from_json(device_property_interface_file_path);
  std::filesystem::path device_aggregated_interface_file_path =
      "samples/interfaces/org.astarte-platform.cpp.examples.DeviceAggregate.json";
  msghub_client->add_interface_from_json(device_aggregated_interface_file_path);
  std::filesystem::path server_aggregated_interface_file_path =
      "samples/interfaces/org.astarte-platform.cpp.examples.ServerAggregate.json";
  msghub_client->add_interface_from_json(server_aggregated_interface_file_path);
  std::filesystem::path server_property_interface_file_path =
      "samples/interfaces/org.astarte-platform.cpp.examples.ServerProperty.json";
  msghub_client->add_interface_from_json(server_property_interface_file_path);

  msghub_client->connect();

  std::this_thread::sleep_for(std::chrono::seconds(3));

  // Start a reception thread for the Astarte device
  auto reception_thread = std::thread(reception_handler, msghub_client);

  {
    std::string interface_name("org.astarte-platform.cpp.examples.DeviceDatastream");
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

    std::string integer_path("/integer_endpoint");
    AstarteIndividual integer_value = AstarteIndividual(43);
    msghub_client->send_individual(interface_name, integer_path, integer_value, &now);

    std::string longinteger_path("/longinteger_endpoint");
    AstarteIndividual longinteger_value = AstarteIndividual(8589934592);
    msghub_client->send_individual(interface_name, longinteger_path, longinteger_value, &now);

    std::string double_path("/double_endpoint");
    AstarteIndividual double_value = AstarteIndividual(43.5);
    msghub_client->send_individual(interface_name, double_path, double_value, &now);

    std::string boolean_path("/boolean_endpoint");
    AstarteIndividual boolean_value = AstarteIndividual(true);
    msghub_client->send_individual(interface_name, boolean_path, boolean_value, &now);

    std::string string_path("/string_endpoint");
    std::string hello_string("Hello from cpp!");
    AstarteIndividual string_value = AstarteIndividual(hello_string);
    msghub_client->send_individual(interface_name, string_path, string_value, &now);

    std::string binaryblob_path("/binaryblob_endpoint");
    std::vector<uint8_t> binaryblob = {10, 20, 30, 40, 50};
    AstarteIndividual binaryblob_value = AstarteIndividual(binaryblob);
    msghub_client->send_individual(interface_name, binaryblob_path, binaryblob_value, &now);

    std::string datetime_path("/datetime_endpoint");
    AstarteIndividual datetime_value = AstarteIndividual(std::chrono::system_clock::now());
    msghub_client->send_individual(interface_name, datetime_path, datetime_value, &now);

    std::string integerarray_path("/integerarray_endpoint");
    std::vector<int32_t> integerarray = {10, 20, 30, 40, 50};
    AstarteIndividual integerarray_value = AstarteIndividual(integerarray);
    msghub_client->send_individual(interface_name, integerarray_path, integerarray_value, &now);

    std::string longintegerarray_path("/longintegerarray_endpoint");
    std::vector<int64_t> longintegerarray = {8589934592, 8589934593, 8589939592};
    AstarteIndividual longintegerarray_value = AstarteIndividual(longintegerarray);
    msghub_client->send_individual(interface_name, longintegerarray_path, longintegerarray_value,
                                   &now);

    std::string doubleararray_path("/doublearray_endpoint");
    std::vector<double> doublearray = {0.0};
    AstarteIndividual doublearray_value = AstarteIndividual(doublearray);
    msghub_client->send_individual(interface_name, doubleararray_path, doublearray_value, &now);

    std::string booleanarray_path("/booleanarray_endpoint");
    std::vector<bool> booleanarray = {true, false, true};
    AstarteIndividual booleanarray_value = AstarteIndividual(booleanarray);
    msghub_client->send_individual(interface_name, booleanarray_path, booleanarray_value, &now);

    std::string stringarray_path("/stringarray_endpoint");
    std::vector<std::string> stringarray = {"Hello ", "world ", "from ", "C++"};
    AstarteIndividual stringarray_value = AstarteIndividual(stringarray);
    msghub_client->send_individual(interface_name, stringarray_path, stringarray_value, &now);

    std::string binaryblobarray_path("/binaryblobarray_endpoint");
    std::vector<std::vector<uint8_t>> binaryblobarray = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    AstarteIndividual binaryblobarray_value = AstarteIndividual(binaryblobarray);
    msghub_client->send_individual(interface_name, binaryblobarray_path, binaryblobarray_value,
                                   &now);

    std::string datetimearray_path("/datetimearray_endpoint");
    std::vector<std::chrono::system_clock::time_point> datetimearray = {
        std::chrono::system_clock::now(), std::chrono::system_clock::now()};
    AstarteIndividual datetimearray_value = AstarteIndividual(datetimearray);
    msghub_client->send_individual(interface_name, datetimearray_path, datetimearray_value, &now);

    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  {
    std::string interface_name("org.astarte-platform.cpp.examples.DeviceAggregate");
    std::string common_path("/sensor15");

    AstarteObject data = {
        {"integer_endpoint", AstarteIndividual(43)},
        {"longinteger_endpoint", AstarteIndividual(8589934592)},
        {"double_endpoint", AstarteIndividual(43.5)},
        {"boolean_endpoint", AstarteIndividual(true)},
        {"string_endpoint", AstarteIndividual(std::string("Hello from cpp!"))},
        {"binaryblob_endpoint", AstarteIndividual(std::vector<uint8_t>{10, 20, 30, 40, 50})},
        {"datetime_endpoint", AstarteIndividual(std::chrono::system_clock::now())},
        {"integerarray_endpoint", AstarteIndividual(std::vector<int32_t>{10, 20, 30, 40, 50})},
        {"longintegerarray_endpoint",
         AstarteIndividual(std::vector<int64_t>{8589934592, 8589934593, 8589939592})},
        {"doublearray_endpoint", AstarteIndividual(std::vector<double>{0.0})},
        {"booleanarray_endpoint", AstarteIndividual(std::vector<bool>{true, false, true})},
        {"stringarray_endpoint",
         AstarteIndividual(std::vector<std::string>{"Hello ", "world ", "from ", "C++"})},
        {"binaryblobarray_endpoint",
         AstarteIndividual(std::vector<std::vector<uint8_t>>{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}})},
        {"datetimearray_endpoint",
         AstarteIndividual(std::vector<std::chrono::system_clock::time_point>{
             std::chrono::system_clock::now(), std::chrono::system_clock::now()})}};
    msghub_client->send_object(interface_name, common_path, data, NULL);

    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  {
    std::string interface_name("org.astarte-platform.cpp.examples.DeviceProperty");

    std::string integer_path("/integer_endpoint");
    AstarteIndividual integer_value = AstarteIndividual(43);
    msghub_client->set_property(interface_name, integer_path, integer_value);

    std::string longinteger_path("/longinteger_endpoint");
    AstarteIndividual longinteger_value = AstarteIndividual(8589934592);
    msghub_client->set_property(interface_name, longinteger_path, longinteger_value);

    std::string double_path("/double_endpoint");
    AstarteIndividual double_value = AstarteIndividual(43.5);
    msghub_client->set_property(interface_name, double_path, double_value);

    std::string boolean_path("/boolean_endpoint");
    AstarteIndividual boolean_value = AstarteIndividual(true);
    msghub_client->set_property(interface_name, boolean_path, boolean_value);

    std::string string_path("/string_endpoint");
    std::string hello_string("Hello from cpp!");
    AstarteIndividual string_value = AstarteIndividual(hello_string);
    msghub_client->set_property(interface_name, string_path, string_value);

    std::string binaryblob_path("/binaryblob_endpoint");
    std::vector<uint8_t> binaryblob = {10, 20, 30, 40, 50};
    AstarteIndividual binaryblob_value = AstarteIndividual(binaryblob);
    msghub_client->set_property(interface_name, binaryblob_path, binaryblob_value);

    std::string datetime_path("/datetime_endpoint");
    AstarteIndividual datetime_value = AstarteIndividual(std::chrono::system_clock::now());
    msghub_client->set_property(interface_name, datetime_path, datetime_value);

    std::string integerarray_path("/integerarray_endpoint");
    std::vector<int32_t> integerarray = {10, 20, 30, 40, 50};
    AstarteIndividual integerarray_value = AstarteIndividual(integerarray);
    msghub_client->set_property(interface_name, integerarray_path, integerarray_value);

    std::string longintegerarray_path("/longintegerarray_endpoint");
    std::vector<int64_t> longintegerarray = {8589934592, 8589934593, 8589939592};
    AstarteIndividual longintegerarray_value = AstarteIndividual(longintegerarray);
    msghub_client->set_property(interface_name, longintegerarray_path, longintegerarray_value);

    std::string doubleararray_path("/doublearray_endpoint");
    std::vector<double> doublearray = {0.0};
    AstarteIndividual doublearray_value = AstarteIndividual(doublearray);
    msghub_client->set_property(interface_name, doubleararray_path, doublearray_value);

    std::string booleanarray_path("/booleanarray_endpoint");
    std::vector<bool> booleanarray = {true, false, true};
    AstarteIndividual booleanarray_value = AstarteIndividual(booleanarray);
    msghub_client->set_property(interface_name, booleanarray_path, booleanarray_value);

    std::string stringarray_path("/stringarray_endpoint");
    std::vector<std::string> stringarray = {"Hello ", "world ", "from ", "C++"};
    AstarteIndividual stringarray_value = AstarteIndividual(stringarray);
    msghub_client->set_property(interface_name, stringarray_path, stringarray_value);

    std::string binaryblobarray_path("/binaryblobarray_endpoint");
    std::vector<std::vector<uint8_t>> binaryblobarray = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    AstarteIndividual binaryblobarray_value = AstarteIndividual(binaryblobarray);
    msghub_client->set_property(interface_name, binaryblobarray_path, binaryblobarray_value);

    std::string datetimearray_path("/datetimearray_endpoint");
    std::vector<std::chrono::system_clock::time_point> datetimearray = {
        std::chrono::system_clock::now(), std::chrono::system_clock::now()};
    AstarteIndividual datetimearray_value = AstarteIndividual(datetimearray);
    msghub_client->set_property(interface_name, datetimearray_path, datetimearray_value);

    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  {
    std::string interface_name("org.astarte-platform.cpp.examples.DeviceProperty");

    std::string integer_path("/integer_endpoint");
    msghub_client->unset_property(interface_name, integer_path);

    std::string longinteger_path("/longinteger_endpoint");
    msghub_client->unset_property(interface_name, longinteger_path);

    std::string double_path("/double_endpoint");
    msghub_client->unset_property(interface_name, double_path);

    std::string boolean_path("/boolean_endpoint");
    msghub_client->unset_property(interface_name, boolean_path);

    std::string string_path("/string_endpoint");
    msghub_client->unset_property(interface_name, string_path);

    std::string binaryblob_path("/binaryblob_endpoint");
    msghub_client->unset_property(interface_name, binaryblob_path);

    std::string datetime_path("/datetime_endpoint");
    msghub_client->unset_property(interface_name, datetime_path);

    std::string integerarray_path("/integerarray_endpoint");
    msghub_client->unset_property(interface_name, integerarray_path);

    std::string longintegerarray_path("/longintegerarray_endpoint");
    msghub_client->unset_property(interface_name, longintegerarray_path);

    std::string doubleararray_path("/doublearray_endpoint");
    msghub_client->unset_property(interface_name, doubleararray_path);

    std::string booleanarray_path("/booleanarray_endpoint");
    msghub_client->unset_property(interface_name, booleanarray_path);

    std::string stringarray_path("/stringarray_endpoint");
    msghub_client->unset_property(interface_name, stringarray_path);

    std::string binaryblobarray_path("/binaryblobarray_endpoint");
    msghub_client->unset_property(interface_name, binaryblobarray_path);

    std::string datetimearray_path("/datetimearray_endpoint");
    msghub_client->unset_property(interface_name, datetimearray_path);

    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  // Wait to receive some messages
  std::this_thread::sleep_for(std::chrono::seconds(20));

  msghub_client->disconnect();

  std::this_thread::sleep_for(std::chrono::seconds(3));
  return 0;
}
