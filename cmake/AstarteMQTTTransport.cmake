# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0
#
# This file contains all functions needed to add MQTT transport support.

include_guard(GLOBAL)

# Defines MQTT-specific CMake options and displays them.
function(astarte_sdk_add_mqtt_options)
    option(ASTARTE_USE_SYSTEM_MQTT "Use the system installed MQTT library" OFF)

    message(STATUS "  MQTT Options:")
    message(STATUS "    ASTARTE_USE_SYSTEM_MQTT: ${ASTARTE_USE_SYSTEM_MQTT}")
endfunction()

# Finds and/or downloads the dependencies required for MQTT transport.
function(astarte_sdk_configure_mqtt_dependencies)
    # Fetch and configure the Paho C++ library
    if(ASTARTE_USE_SYSTEM_MQTT)
        find_package(PahoMqttCpp REQUIRED)
    else()
        FetchContent_Declare(
            paho-mqtt-cpp
            GIT_REPOSITORY https://github.com/eclipse/paho.mqtt.cpp.git
            GIT_TAG v1.5.3
        )

        # Set build options
        set(PAHO_BUILD_EXAMPLES OFF CACHE BOOL "")
        set(PAHO_BUILD_TESTS OFF CACHE BOOL "")
        set(PAHO_WITH_MQTT_C ON CACHE BOOL "")

        FetchContent_MakeAvailable(paho-mqtt-cpp)
    endif()
endfunction()

# Adds MQTT source files and links required libraries to the main target.
function(astarte_sdk_add_mqtt_transport)
    # Add MQTT transport sources
    file(GLOB astarte_sdk_src_transport "${CMAKE_CURRENT_LIST_DIR}/src/mqtt/*.cpp")
    target_sources(astarte_device_sdk PRIVATE ${astarte_sdk_src_transport})

    # Link with MQTT
    if(ASTARTE_USE_SYSTEM_MQTT)
        target_link_libraries(astarte_device_sdk PRIVATE PahoMqttCpp::paho-mqttpp3-static)
    else()
        target_link_libraries(astarte_device_sdk PRIVATE PahoMqttCpp::paho-mqttpp3)
    endif()
endfunction()

# Creates and installs the pkg-config file for the mqtt-enabled SDK.
function(astarte_sdk_install_mqtt_pkgconfig)
    # TODO: implement the install steps
endfunction()
