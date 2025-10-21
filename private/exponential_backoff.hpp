// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef EXPONENTIAL_BACKOFF_H
#define EXPONENTIAL_BACKOFF_H

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <random>

#include "astarte_device_sdk/exceptions.hpp"

namespace AstarteDeviceSdk {

class ExponentialBackoff {
 public:
  /**
   * @brief Construct an ExponentialBackoff instance.
   *
   * @details The exponential backoff will will compute an exponential delay using
   * the 2 as the base for the power operation.
   * As such the values returned by calls to getNextDelay will follow the formula:
   * min( @p initial_delay * 2 ^ ( number of calls ) , @p max_delay ) + random jitter
   * The random jitter will be in the range [ - @p initial_delay , + @p initial_delay )
   *
   * @note Since the exponential backoff will be computed using @p initial_delay as a base,
   * if that parameter is zero, then the next delay will always be zero.
   * @note The jitter will be applied also once the @p max_delay has been reached. Effectively the
   * maximum delay produced will be @p max_delay + @p initial_delay.
   *
   * @param initial_delay The value for the first backoff delay.
   * @param max_delay The upper bound for all the backoff delays.
   */
  ExponentialBackoff(std::chrono::milliseconds initial_delay, std::chrono::milliseconds max_delay)
      : initial_delay_(initial_delay), max_delay_(max_delay) {
    if ((initial_delay < std::chrono::milliseconds::zero()) ||
        (max_delay < std::chrono::milliseconds::zero())) {
      throw AstarteInvalidInputException(
          "Negative duration passed to the ExponentialBackoff constructor");
    }
    if (max_delay < initial_delay) {
      throw AstarteInvalidInputException(
          "Maximum delay should be larger or equal to the initial delay");
    }
  }

  /**
   * @brief Calculate and returns the next backoff delay.
   * @details See the documentation of the constructor of this class for an explanation on how
   * this delay is computed.
   * @return The calculated delay duration.
   */
  auto getNextDelay() -> std::chrono::milliseconds {
    if (initial_delay_ == std::chrono::milliseconds::zero()) {
      return initial_delay_;
    }

    const ChronoMillisRep initial_delay = initial_delay_.count();
    const ChronoMillisRep max_milliseconds = std::chrono::milliseconds::max().count();
    const ChronoMillisRep max_allowed_last_delay = (max_milliseconds - initial_delay) / 2;

    // Update last delay value with the new value
    if (last_delay_ == 0) {
      last_delay_ = initial_delay;
    } else {
      if (last_delay_ > max_allowed_last_delay) {
        last_delay_ = max_milliseconds - initial_delay;
      } else {
        last_delay_ = 2 * last_delay_;
      }
    }

    // Bound the delay to the maximum
    ChronoMillisRep bounded_delay = std::min(last_delay_, max_delay_.count());

    // Insert some jitter
    std::uniform_int_distribution<ChronoMillisRep> dist(-initial_delay, initial_delay);
    ChronoMillisRep jittered_delay = bounded_delay + dist(gen_);

    // Convert to a chrono object
    return std::chrono::milliseconds(jittered_delay);
  }

  /** @brief Reset the backoff generator. */
  void reset() { last_delay_ = 0; }

 private:
  using ChronoMillisRep = std::chrono::milliseconds::rep;

  std::chrono::milliseconds initial_delay_;
  std::chrono::milliseconds max_delay_;
  std::random_device rd_;
  std::mt19937 gen_{rd_()};
  ChronoMillisRep last_delay_{0};
};

}  // namespace AstarteDeviceSdk

#endif  // EXPONENTIAL_BACKOFF_H
