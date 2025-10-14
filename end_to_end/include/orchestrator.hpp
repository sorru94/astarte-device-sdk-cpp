// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <spdlog/spdlog.h>

#include <chrono>
#include <functional>
#include <variant>
#include <vector>

#if defined(ASTARTE_TRANSPORT_GRPC)
#include "astarte_device_sdk/device_grpc.hpp"
#else
#include "astarte_device_sdk/device_mqtt.hpp"
#endif
#include "case.hpp"

using namespace std::chrono_literals;

#if defined(ASTARTE_TRANSPORT_GRPC)
using AstarteDeviceSdk::AstarteDeviceGRPC;
#else
using AstarteDeviceSdk::AstarteDeviceMQTT;
#endif

struct GRPCConfig {
  std::string server_addr;
  std::string node_id;
  std::vector<std::filesystem::path> interfaces;
};

struct MQTTConfig {
  std::vector<std::filesystem::path> interfaces;
};

struct CURLConfig {
  std::string appengine_url;
  std::string appengine_token;
  std::string realm;
  std::string device_id;
};

// Orchestrator for end to end tests
class TestOrchestrator {
 public:
  explicit TestOrchestrator(
      const std::variant<struct GRPCConfig, struct MQTTConfig>& transport_config,
      const struct CURLConfig& config_curl)
      : transport_config_(transport_config), curl_config_(config_curl) {}

  // Add test case to orchestrator
  void add_test_case(TestCase&& tc) { test_cases_.push(std::move(tc)); }

  // Execute all test cases
  void execute_all() {
    spdlog::info("Executing all end to end test cases...");

    // Execute each test case sequentially cleaning the test cases queue in the process
    for (; !test_cases_.empty(); test_cases_.pop()) {
      TestCase test_case = std::move(test_cases_.front());

#if defined(ASTARTE_TRANSPORT_GRPC)
      auto config_grpc = std::get<struct GRPCConfig>(transport_config_);
      std::shared_ptr<AstarteDeviceGRPC> device_grpc =
          std::make_shared<AstarteDeviceGRPC>(config_grpc.server_addr, config_grpc.node_id);
      for (const std::filesystem::path& interface_path : config_grpc.interfaces) {
        device_grpc->add_interface_from_file(interface_path);
      }
      test_case.attach_device(device_grpc);
#else
      // TODO: add MQTT device
#endif

      test_case.configure_curl(curl_config_.appengine_url, curl_config_.appengine_token,
                               curl_config_.realm, curl_config_.device_id);

      // run the isolated test
      test_case.start();
      test_case.execute();
    }
  }

 private:
  std::variant<struct GRPCConfig, struct MQTTConfig> transport_config_;
  struct CURLConfig curl_config_;
  std::queue<TestCase> test_cases_;
};
