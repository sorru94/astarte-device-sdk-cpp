// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>
#include <chrono>

#include "astarte_device_sdk/individual.hpp"

using AstarteDeviceSdk::AstarteIndividual;

using testing::ContainerEq;

TEST(InidividualTest, InstantiationBoolean) {
  bool value = true;
  auto individual = AstarteIndividual(value);
  auto original = individual.into<bool>();
  EXPECT_EQ(value, original);
}

TEST(InidividualTest, InstantiationInteger) {
  int32_t value = 52;
  auto individual = AstarteIndividual(value);
  auto original = individual.into<int32_t>();
  EXPECT_EQ(value, original);
}

TEST(InidividualTest, InstantiationLongInteger) {
  int64_t value = 52;
  auto individual = AstarteIndividual(value);
  auto original = individual.into<int64_t>();
  EXPECT_EQ(value, original);
}

TEST(InidividualTest, InstantiationString) {
  std::string value = "Test string";
  auto individual = AstarteIndividual(value);
  auto original = individual.into<std::string>();
  EXPECT_STREQ(value.c_str(), original.c_str());
}

TEST(InidividualTest, InstantiationBinaryBlob) {
  std::vector<uint8_t> value = {0x12U, 0x22U, 0x42};
  auto individual = AstarteIndividual(value);
  auto original = individual.into<std::vector<uint8_t>>();
  EXPECT_THAT(value, ContainerEq(original));
}

TEST(InidividualTest, InstantiationDatetime) {
  std::tm date_tm = {};
  date_tm.tm_year = 1994 - 1900;
  date_tm.tm_mon = 4 - 1;
  date_tm.tm_mday = 12;
  std::time_t time = std::mktime(&date_tm);
  std::chrono::system_clock::time_point value = std::chrono::system_clock::from_time_t(time);
  auto individual = AstarteIndividual(value);
  auto original = individual.into<std::chrono::system_clock::time_point>();
  EXPECT_EQ(time, std::chrono::system_clock::to_time_t(original));
}

TEST(InidividualTest, InstantiationDouble) {
  double value = 43.5;
  auto individual = AstarteIndividual(value);
  auto original = individual.into<double>();
  EXPECT_FLOAT_EQ(value, original);
}
