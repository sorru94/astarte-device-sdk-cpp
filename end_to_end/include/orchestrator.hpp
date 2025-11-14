// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <spdlog/spdlog.h>

#include <chrono>
#include <functional>
#include <vector>

#include "case.hpp"

using namespace std::chrono_literals;

struct ConfigGRPC {
  std::string server_addr;
  std::string node_id;
  std::vector<std::filesystem::path> interfaces;
};

struct ConfigCURL {
  std::string appengine_url;
  std::string appengine_token;
  std::string realm;
  std::string device_id;
};

// Orchestrator for end to end tests
class TestOrchestrator {
 public:
  explicit TestOrchestrator(const struct ConfigGRPC& config_grpc,
                            const struct ConfigCURL& config_curl)
      : grpc_config_(config_grpc), curl_config_(config_curl) {}

  // Add test case factory to orchestrator
  void add_test_case(TestCase&& tc) { test_cases_.push(std::move(tc)); }

  // Execute all test cases by creating them on the fly
  void execute_all() {
    spdlog::info("Executing all end to end test cases...");

    // remove testcases from queue so that at each iteration the TestCase resources
    // are freed
    for (; !test_cases_.empty(); test_cases_.pop()) {
      TestCase test_case = std::move(test_cases_.front());

      // create a new device and attach it
      std::shared_ptr<AstarteDeviceGRPC> device =
          std::make_shared<AstarteDeviceGRPC>(grpc_config_.server_addr, grpc_config_.node_id);
      for (const std::filesystem::path& interface_path : grpc_config_.interfaces) {
        auto res = device->add_interface_from_file(interface_path);
        if (!res) {
          throw EndToEndAstarteDeviceException(astarte_fmt::format("{}", res.error()));
        }
      }
      test_case.attach_device(device);
      test_case.configure_curl(curl_config_.appengine_url, curl_config_.appengine_token,
                               curl_config_.realm, curl_config_.device_id);

      // run the isolated test
      test_case.start();
      test_case.execute();
    }
  }

 private:
  struct ConfigGRPC grpc_config_;
  struct ConfigCURL curl_config_;
  std::queue<TestCase> test_cases_;
};
