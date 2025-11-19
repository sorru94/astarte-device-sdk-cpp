// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "action.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"

using AstarteDeviceSdk::AstarteError;
using AstarteDeviceSdk::PairingApi;

constexpr size_t CREDENTIAL_SECRET_LEN = 44;

class TestActionPairingApiRegistration : public TestAction {
 public:
  static std::shared_ptr<TestActionPairingApiRegistration> Create(std::string pairing_token) {
    return std::shared_ptr<TestActionPairingApiRegistration>(
        new TestActionPairingApiRegistration(pairing_token));
  }

  auto execute_unchecked(const std::string& case_name) const
      -> AstarteDeviceSdk::astarte_tl::expected<void, AstarteError> override {
    spdlog::info("[{}] Pairing device...", case_name);
    auto res =
        PairingApi::create(realm_, device_id_, astarte_base_url_)
            .and_then([&](const PairingApi& pairing_api)
                          -> AstarteDeviceSdk::astarte_tl::expected<std::string, AstarteError> {
              return pairing_api.register_device(pairing_token_);
            });
    if (!res) {
      return AstarteDeviceSdk::astarte_tl::unexpected(res.error());
    }

    if (res.value().length() != CREDENTIAL_SECRET_LEN) {
      spdlog::error("Expected: {}", CREDENTIAL_SECRET_LEN);
      spdlog::error("Actual: {}", res.value().length());
      throw EndToEndMismatchException("Incorrect length for the credential secret.");
    }
    return {};
  }

 private:
  TestActionPairingApiRegistration(std::string pairing_token) : pairing_token_(pairing_token) {}
  std::string pairing_token_;
};
