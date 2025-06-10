// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "case.hpp"
#include "constants/astarte_interfaces.hpp"

namespace testcases {
TestCase device_datastream() {
  return TestCase(
      "Device to Astarte",
      std::vector<std::shared_ptr<TestAction>>{
          TestActionConnect::Create(),
          TestActionSleep::Create(std::chrono::seconds(1)),
          TestActionTransmitMQTTData::Create(
              AstarteMessage(astarte_interfaces::DEVICE_DATASTREAM, "/integer_endpoint",
                             AstarteDatastreamIndividual(AstarteData(12)))),
          TestActionTransmitMQTTData::Create(
              AstarteMessage(astarte_interfaces::DEVICE_DATASTREAM, "/longinteger_endpoint",
                             AstarteDatastreamIndividual(AstarteData(17179869184)))),
          TestActionTransmitMQTTData::Create(
              AstarteMessage(astarte_interfaces::DEVICE_DATASTREAM, "/double_endpoint",
                             AstarteDatastreamIndividual(AstarteData(54.4)))),
          TestActionTransmitMQTTData::Create(
              AstarteMessage(astarte_interfaces::DEVICE_DATASTREAM, "/boolean_endpoint",
                             AstarteDatastreamIndividual(AstarteData(true)))),
          TestActionTransmitMQTTData::Create(
              AstarteMessage(astarte_interfaces::DEVICE_DATASTREAM, "/string_endpoint",
                             AstarteDatastreamIndividual(AstarteData(std::string("Hello C++!"))))),
          TestActionTransmitMQTTData::Create(AstarteMessage(
              astarte_interfaces::DEVICE_DATASTREAM, "/datetime_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::chrono::system_clock::time_point(
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                  std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0)))))),
          TestActionTransmitMQTTData::Create(AstarteMessage(
              astarte_interfaces::DEVICE_DATASTREAM, "/binaryblob_endpoint",

              AstarteDatastreamIndividual(AstarteData(std::vector<uint8_t>{0x23, 0x43, 0xF5})))),
          TestActionTransmitMQTTData::Create(AstarteMessage(
              astarte_interfaces::DEVICE_DATASTREAM, "/integerarray_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::vector<int32_t>{13, 2})))),
          TestActionTransmitMQTTData::Create(AstarteMessage(
              astarte_interfaces::DEVICE_DATASTREAM, "/longintegerarray_endpoint",

              AstarteDatastreamIndividual(AstarteData(std::vector<int64_t>{17179869184, 5})))),
          TestActionTransmitMQTTData::Create(
              AstarteMessage(astarte_interfaces::DEVICE_DATASTREAM, "/doublearray_endpoint",
                             AstarteDatastreamIndividual(AstarteData(std::vector<double>{0.5})))),
          TestActionTransmitMQTTData::Create(AstarteMessage(
              astarte_interfaces::DEVICE_DATASTREAM, "/booleanarray_endpoint",

              AstarteDatastreamIndividual(AstarteData(std::vector<bool>{false, true})))),
          TestActionTransmitMQTTData::Create(AstarteMessage(
              astarte_interfaces::DEVICE_DATASTREAM, "/stringarray_endpoint",

              AstarteDatastreamIndividual(
                  AstarteData(std::vector<std::string>{"Hello ", "world ", "from ", "C++"})))),
          TestActionTransmitMQTTData::Create(AstarteMessage(
              astarte_interfaces::DEVICE_DATASTREAM, "/datetimearray_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::vector<
                                                      std::chrono::system_clock::time_point>{
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                      std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0),
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1985), std::chrono::month(5), std::chrono::day(22))) +
                      std::chrono::hours(0) + std::chrono::minutes(0) + std::chrono::seconds(12),
              })))),
          TestActionTransmitMQTTData::Create(AstarteMessage(
              astarte_interfaces::DEVICE_DATASTREAM, "/binaryblobarray_endpoint",

              AstarteDatastreamIndividual(AstarteData(
                  std::vector<std::vector<uint8_t>>{{0x23, 0x43, 0xF5}, {0x43, 0xF3, 0x00}})))),
          TestActionSleep::Create(std::chrono::seconds(1)),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DEVICE_DATASTREAM, "integer_endpoint",
                             AstarteDatastreamIndividual(AstarteData(12)))),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DEVICE_DATASTREAM, "longinteger_endpoint",
                             AstarteDatastreamIndividual(AstarteData(17179869184)))),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DEVICE_DATASTREAM, "double_endpoint",
                             AstarteDatastreamIndividual(AstarteData(54.4)))),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DEVICE_DATASTREAM, "boolean_endpoint",
                             AstarteDatastreamIndividual(AstarteData(true)))),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DEVICE_DATASTREAM, "string_endpoint",
                             AstarteDatastreamIndividual(AstarteData(std::string("Hello C++!"))))),
          TestActionFetchRESTData::Create(AstarteMessage(
              astarte_interfaces::DEVICE_DATASTREAM, "datetime_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::chrono::system_clock::time_point(
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                  std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0)))))),
          TestActionFetchRESTData::Create(AstarteMessage(
              astarte_interfaces::DEVICE_DATASTREAM, "binaryblob_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::vector<uint8_t>{0x23, 0x43, 0xF5})))),
          TestActionFetchRESTData::Create(AstarteMessage(
              astarte_interfaces::DEVICE_DATASTREAM, "integerarray_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::vector<int32_t>{13, 2})))),
          TestActionFetchRESTData::Create(AstarteMessage(
              astarte_interfaces::DEVICE_DATASTREAM, "longintegerarray_endpoint",

              AstarteDatastreamIndividual(AstarteData(std::vector<int64_t>{17179869184, 5})))),
          TestActionFetchRESTData::Create(
              AstarteMessage(astarte_interfaces::DEVICE_DATASTREAM, "doublearray_endpoint",
                             AstarteDatastreamIndividual(AstarteData(std::vector<double>{0.5})))),
          TestActionFetchRESTData::Create(AstarteMessage(
              astarte_interfaces::DEVICE_DATASTREAM, "booleanarray_endpoint",

              AstarteDatastreamIndividual(AstarteData(std::vector<bool>{false, true})))),
          TestActionFetchRESTData::Create(AstarteMessage(
              astarte_interfaces::DEVICE_DATASTREAM, "stringarray_endpoint",

              AstarteDatastreamIndividual(
                  AstarteData(std::vector<std::string>{"Hello ", "world ", "from ", "C++"})))),
          TestActionFetchRESTData::Create(AstarteMessage(
              astarte_interfaces::DEVICE_DATASTREAM, "datetimearray_endpoint",
              AstarteDatastreamIndividual(AstarteData(std::vector<
                                                      std::chrono::system_clock::time_point>{
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                      std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0),
                  std::chrono::sys_days(std::chrono::year_month_day(
                      std::chrono::year(1985), std::chrono::month(5), std::chrono::day(22))) +
                      std::chrono::hours(0) + std::chrono::minutes(0) + std::chrono::seconds(12),
              })))),
          TestActionFetchRESTData::Create(AstarteMessage(
              astarte_interfaces::DEVICE_DATASTREAM, "binaryblobarray_endpoint",

              AstarteDatastreamIndividual(AstarteData(
                  std::vector<std::vector<uint8_t>>{{0x23, 0x43, 0xF5}, {0x43, 0xF3, 0x00}})))),
          TestActionSleep::Create(std::chrono::seconds(1)),
          TestActionDisconnect::Create(),
          TestActionSleep::Create(std::chrono::seconds(1))});
}
}  // namespace testcases
