// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "exponential_backoff.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::AllOf;
using ::testing::Ge;
using ::testing::Le;

using AstarteDeviceSdk::ExponentialBackoff;

TEST(AstarteTestExponentialBackoff, IncorrectInputs) {
  EXPECT_THROW(ExponentialBackoff backoff(std::chrono::minutes(-1), std::chrono::minutes(1)),
               std::exception);
  EXPECT_THROW(ExponentialBackoff backoff(std::chrono::minutes(1), std::chrono::minutes(-1)),
               std::exception);
  EXPECT_THROW(
      ExponentialBackoff backoff(std::chrono::minutes(1), std::chrono::milliseconds::zero()),
      std::exception);
}

TEST(AstarteTestExponentialBackoff, InitialDelayZero) {
  ExponentialBackoff backoff(std::chrono::milliseconds::zero(), std::chrono::minutes(1));
  for (size_t i = 0; i < 1024u; i++) {
    EXPECT_EQ(backoff.getNextDelay(), std::chrono::milliseconds::zero());
  }
}

TEST(AstarteTestExponentialBackoff, OrdinaryBackoff) {
  ExponentialBackoff backoff(std::chrono::minutes(1), std::chrono::minutes(18));
  EXPECT_THAT(backoff.getNextDelay(),
              AllOf(Ge(std::chrono::milliseconds::zero()), Le(std::chrono::minutes(2))));
  EXPECT_THAT(backoff.getNextDelay(),
              AllOf(Ge(std::chrono::minutes(1)), Le(std::chrono::minutes(3))));
  EXPECT_THAT(backoff.getNextDelay(),
              AllOf(Ge(std::chrono::minutes(3)), Le(std::chrono::minutes(5))));
  EXPECT_THAT(backoff.getNextDelay(),
              AllOf(Ge(std::chrono::minutes(7)), Le(std::chrono::minutes(9))));
  EXPECT_THAT(backoff.getNextDelay(),
              AllOf(Ge(std::chrono::minutes(15)), Le(std::chrono::minutes(17))));
  for (size_t i = 0; i < 1048576u; i++) {
    EXPECT_THAT(backoff.getNextDelay(),
                AllOf(Ge(std::chrono::minutes(17)), Le(std::chrono::minutes(19))));
  }
}
