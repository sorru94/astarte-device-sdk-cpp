// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef INDIVIDUAL_PRIVATE_H
#define INDIVIDUAL_PRIVATE_H

#include <astarteplatform/msghub/astarte_type.pb.h>

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace AstarteDeviceSdk {

using astarteplatform::msghub::AstarteDataTypeIndividual;

auto astarte_individual_to_astarte_data_type_individual(int32_t value)
    -> AstarteDataTypeIndividual *;
auto astarte_individual_to_astarte_data_type_individual(int64_t value)
    -> AstarteDataTypeIndividual *;
auto astarte_individual_to_astarte_data_type_individual(double value)
    -> AstarteDataTypeIndividual *;
auto astarte_individual_to_astarte_data_type_individual(bool value) -> AstarteDataTypeIndividual *;
auto astarte_individual_to_astarte_data_type_individual(std::string value)
    -> AstarteDataTypeIndividual *;
auto astarte_individual_to_astarte_data_type_individual(std::vector<uint8_t> value)
    -> AstarteDataTypeIndividual *;
auto astarte_individual_to_astarte_data_type_individual(std::chrono::system_clock::time_point value)
    -> AstarteDataTypeIndividual *;
auto astarte_individual_to_astarte_data_type_individual(const std::vector<int32_t> &values)
    -> AstarteDataTypeIndividual *;
auto astarte_individual_to_astarte_data_type_individual(const std::vector<int64_t> &values)
    -> AstarteDataTypeIndividual *;
auto astarte_individual_to_astarte_data_type_individual(const std::vector<double> &values)
    -> AstarteDataTypeIndividual *;
auto astarte_individual_to_astarte_data_type_individual(const std::vector<bool> &values)
    -> AstarteDataTypeIndividual *;
auto astarte_individual_to_astarte_data_type_individual(const std::vector<std::string> &values)
    -> AstarteDataTypeIndividual *;
auto astarte_individual_to_astarte_data_type_individual(
    const std::vector<std::vector<uint8_t>> &values) -> AstarteDataTypeIndividual *;
auto astarte_individual_to_astarte_data_type_individual(
    const std::vector<std::chrono::system_clock::time_point> &values)
    -> AstarteDataTypeIndividual *;

}  // namespace AstarteDeviceSdk

#endif  // INDIVIDUAL_PRIVATE_H
