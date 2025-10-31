// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_ERRORS_H
#define ASTARTE_DEVICE_SDK_ERRORS_H

#include <string>
#include <variant>

#if defined(ASTARTE_USE_TL_EXPECTED)
#include "tl/expected.hpp"
#else
#include <expected>
#endif

namespace AstarteDeviceSdk {

#if defined(ASTARTE_USE_TL_EXPECTED)
namespace std_alt_tl = ::tl;
#else
namespace std_alt_tl = ::std;
#endif

/**
 * @brief Base error class representing any possible error from the Astarte device library.
 *
 * This type is intended to be used as the error type 'E' in std::expected<T, E>.
 */
class AstarteErrorBase {
 protected:
  /**
   * @brief Constructor for the Astarte error.
   * @param message The error message.
   */
  explicit AstarteErrorBase(std::string_view message) : message_(message) {}

 public:
  /**
   * @brief Return the message encapsulated in the error.
   * @return The error message.
   */
  auto message() -> std::string& { return message_; }

 private:
  std::string message_;
};

/**
 * @brief Specific error for when a file cannot be opened.
 */
class AstarteFileOpenError : public AstarteErrorBase {
 public:
  /**
   * @brief Constructor for the Astarte file open error.
   * @param filename The file which could not be opened.
   */
  explicit AstarteFileOpenError(std::string_view filename)
      : AstarteErrorBase("Failed to open file: " + std::string(filename)) {}
};

/**
 * @brief Specific error for when an operation failed due to incompatible user input.
 */
class AstarteInvalidInputError : public AstarteErrorBase {
 public:
  /**
   * @brief Constructor for the Astarte incompatible input error.
   * @param message The error message.
   */
  explicit AstarteInvalidInputError(std::string_view message) : AstarteErrorBase(message) {}
};

/**
 * @brief Specific error for when an operation failed due to incompatible user input.
 */
class AstarteInternalError : public AstarteErrorBase {
 public:
  /**
   * @brief Constructor for an internal operation failure.
   * @param message The error message.
   */
  explicit AstarteInternalError(std::string_view message) : AstarteErrorBase(message) {}
};

/**
 * @brief Specific error for when an operation failed due to incompatible user input.
 */
class AstarteOperationRefusedError : public AstarteErrorBase {
 public:
  /**
   * @brief Constructor for an operation refused by Astarte.
   * @param message The error message.
   */
  explicit AstarteOperationRefusedError(std::string_view message) : AstarteErrorBase(message) {}
};

/**
 * @brief A variant type representing any possible error from the Astarte device library.
 *
 * This type is intended to be used as the error type 'E' in std::expected<T, E>.
 */
using AstarteError = std::variant<AstarteFileOpenError, AstarteInvalidInputError,
                                  AstarteInternalError, AstarteOperationRefusedError>;

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_ERRORS_H
