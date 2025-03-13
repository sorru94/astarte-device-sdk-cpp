// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "astarte_device_sdk/individual.hpp"
#include "grpc_converter.hpp"

using AstarteDeviceSdk::AstarteIndividual;
using AstarteDeviceSdk::gRPCAstarteDataTypeIndividual;
using AstarteDeviceSdk::GrpcConverter;

TEST(ConversionTest, IndividualToGRPC) {
  int32_t value = 199;
  auto individual = AstarteIndividual(value);
  gRPCAstarteDataTypeIndividual *grpc_individual =
      std::visit(GrpcConverter(), individual.get_raw_data());
  EXPECT_EQ(grpc_individual->individual_data_case(),
            gRPCAstarteDataTypeIndividual::kAstarteInteger);
  EXPECT_EQ(grpc_individual->astarte_integer(), value);
  GrpcConverter converter;
  AstarteIndividual original = converter(*grpc_individual);
  EXPECT_EQ(original.into<int32_t>(), value);
}
