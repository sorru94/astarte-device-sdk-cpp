// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/errors.hpp"

#include <memory>
#include <string_view>
#include <variant>

namespace AstarteDeviceSdk {

AstarteErrorBase::AstarteErrorBase(std::string_view type, std::string_view message)
    : type_(type), message_(message), other_(nullptr) {}
AstarteErrorBase::AstarteErrorBase(std::string_view type, std::string_view message,
                                   const AstarteErrorBase& other)
    : type_(type), message_(message), other_(std::make_shared<AstarteErrorBase>(other)) {}
AstarteErrorBase::~AstarteErrorBase() = default;
auto AstarteErrorBase::message() const -> const std::string& { return message_; }
auto AstarteErrorBase::type() const -> const std::string& { return type_; }
auto AstarteErrorBase::nested_error() const -> const std::shared_ptr<AstarteErrorBase>& {
  return other_;
}

AstarteInternalError::AstarteInternalError(std::string_view message)
    : AstarteErrorBase(k_type_, message) {}
AstarteInternalError::AstarteInternalError(std::string_view message, const AstarteError& other)
    : AstarteErrorBase(
          k_type_, message,
          std::visit([](const auto& err) -> const AstarteErrorBase& { return err; }, other)) {}

AstarteFileOpenError::AstarteFileOpenError(std::string_view filename)
    : AstarteErrorBase(k_type_, filename) {}
AstarteFileOpenError::AstarteFileOpenError(std::string_view filename, const AstarteError& other)
    : AstarteErrorBase(
          k_type_, filename,
          std::visit([](const auto& err) -> const AstarteErrorBase& { return err; }, other)) {}

AstarteInvalidInputError::AstarteInvalidInputError(std::string_view message)
    : AstarteErrorBase(k_type_, message) {}
AstarteInvalidInputError::AstarteInvalidInputError(std::string_view message,
                                                   const AstarteError& other)
    : AstarteErrorBase(
          k_type_, message,
          std::visit([](const auto& err) -> const AstarteErrorBase& { return err; }, other)) {}

AstarteOperationRefusedError::AstarteOperationRefusedError(std::string_view message)
    : AstarteErrorBase(k_type_, message) {}
AstarteOperationRefusedError::AstarteOperationRefusedError(std::string_view message,
                                                           const AstarteError& other)
    : AstarteErrorBase(
          k_type_, message,
          std::visit([](const auto& err) -> const AstarteErrorBase& { return err; }, other)) {}

AstarteGrpcLibError::AstarteGrpcLibError(std::string_view message)
    : AstarteErrorBase(k_type_, message) {}
AstarteGrpcLibError::AstarteGrpcLibError(std::string_view message, const AstarteError& other)
    : AstarteErrorBase(
          k_type_, message,
          std::visit([](const auto& err) -> const AstarteErrorBase& { return err; }, other)) {}
AstarteGrpcLibError::AstarteGrpcLibError(std::uint64_t code, std::string_view message)
    : AstarteErrorBase(
          k_type_, "code(" + std::to_string(code) + ")-message(" + std::string(message) + ")") {}
AstarteGrpcLibError::AstarteGrpcLibError(std::uint64_t code, std::string_view message,
                                         const AstarteError& other)
    : AstarteErrorBase(
          k_type_, "code(" + std::to_string(code) + ")-message(" + std::string(message) + ")",
          std::visit([](const auto& err) -> const AstarteErrorBase& { return err; }, other)) {}

AstarteMsgHubError::AstarteMsgHubError(std::string_view message)
    : AstarteErrorBase(k_type_, message) {}
AstarteMsgHubError::AstarteMsgHubError(std::string_view message, const AstarteError& other)
    : AstarteErrorBase(
          k_type_, message,
          std::visit([](const auto& err) -> const AstarteErrorBase& { return err; }, other)) {}

}  // namespace AstarteDeviceSdk
