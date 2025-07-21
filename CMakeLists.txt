# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.15)

project(
  AstarteDeviceSDKcpp
  VERSION 0.6.0
  LANGUAGES CXX)

include(FetchContent)

# Setup libraries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

# Configuration options for this library
option(ASTARTE_PUBLIC_SPDLOG_DEP "Make spdlog dependency public" OFF)
option(ASTARTE_PUBLIC_PROTO_DEP "Make message hub proto dependency public" OFF)
set(ASTARTE_MESSAGE_HUB_PROTO_DIR CACHE PATH "Directory to an already downloaded astarte-message-hub-proto repository")

# Set formatting option
# case 1: C++20 (or greater) + compiler with <format> library. Use std format. (default case)
# case 2: C++20 (or greater) + compiler without <format> library. Use fmtlib
# case 3: C++17. Use fmtlib
option(ASTARTE_ENABLE_FORMAT "Enable formatting of Astarte device data" OFF)
if(CMAKE_CXX_STANDARD LESS 20)
# case 3
    set(ASTARTE_ENABLE_FORMAT ON)
else()
    # check if std::format is actually supported.
    include(CheckCXXSourceCompiles)
    check_cxx_source_compiles(
        "
        #include <format>
        int main() {
            auto s = std::format(\"Hello {}\", 42);
            return 0;
        }
        "
        HAS_STD_FORMAT
    )

    if(NOT HAS_STD_FORMAT)
        # case 2
        set(ASTARTE_ENABLE_FORMAT ON)
    else()
        # C++20 with <format> available to compiler requires to enable the follwoing spdlog feature
        # TODO: remove once the minimum required C++ standard becomes C++20
        set(SPDLOG_USE_STD_FORMAT true)
    endif()
endif()


message(STATUS "--------------------------------------------------")
message(STATUS "Astarte SDK Configuration:")
message(STATUS "  ASTARTE_ENABLE_FORMAT:           ${ASTARTE_ENABLE_FORMAT}")
message(STATUS "  ASTARTE_PUBLIC_SPDLOG_DEP:       ${ASTARTE_PUBLIC_SPDLOG_DEP}")
message(STATUS "  ASTARTE_PUBLIC_PROTO_DEP:        ${ASTARTE_PUBLIC_PROTO_DEP}")
message(STATUS "  ASTARTE_MESSAGE_HUB_PROTO_DIR:   ${ASTARTE_MESSAGE_HUB_PROTO_DIR}")
message(STATUS "--------------------------------------------------")

# Logging library
set(SPDLOG_GITHUB_URL https://github.com/gabime/spdlog.git)
set(SPDLOG_GIT_TAG v1.15.1)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY ${SPDLOG_GITHUB_URL}
  GIT_TAG        ${SPDLOG_GIT_TAG}
  SYSTEM
)
FetchContent_MakeAvailable(spdlog)

# Astarte message hub protos
if(ASTARTE_MESSAGE_HUB_PROTO_DIR)
    add_subdirectory(${ASTARTE_MESSAGE_HUB_PROTO_DIR} astarte_msghub_proto)
else()
    set(MSGHUB_PROTO_GITHUB_URL https://github.com/astarte-platform/astarte-message-hub-proto.git)
    set(MSGHUB_PROTO_GIT_TAG v0.8.4)
    FetchContent_Declare(
        astarte_msghub_proto
        GIT_REPOSITORY ${MSGHUB_PROTO_GITHUB_URL}
        GIT_TAG        ${MSGHUB_PROTO_GIT_TAG}
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
    )
    FetchContent_MakeAvailable(astarte_msghub_proto)
endif()

# Create a library from the source code
add_library(astarte_device_sdk)
target_compile_features(astarte_device_sdk PUBLIC cxx_std_17)
target_include_directories(astarte_device_sdk PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_include_directories(astarte_device_sdk PRIVATE ${CMAKE_CURRENT_LIST_DIR}/private)
file(GLOB astarte_sdk_src "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")
target_sources(astarte_device_sdk PRIVATE ${astarte_sdk_src})

# Link with the msghub grpc
target_link_libraries(astarte_device_sdk
    PRIVATE
        ${_GRPC_CPP}
        ${_REFLECTION}
        ${_PROTOBUF_LIBPROTOBUF}
)

# Link the dependencies within the library
if(ASTARTE_ENABLE_FORMAT)
    target_compile_definitions(astarte_device_sdk PUBLIC ASTARTE_FORMAT_ENABLED)
endif()
if(ASTARTE_PUBLIC_SPDLOG_DEP)
    target_link_libraries(astarte_device_sdk PUBLIC spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
else()
    target_link_libraries(astarte_device_sdk PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
endif()
if(ASTARTE_PUBLIC_PROTO_DEP)
    target_link_libraries(astarte_device_sdk PUBLIC astarte_msghub_proto)
else()
    target_link_libraries(astarte_device_sdk PRIVATE astarte_msghub_proto)
endif()

# Set public headers to install
file(GLOB_RECURSE SDK_PUBLIC_HEADERS "${CMAKE_CURRENT_LIST_DIR}/include/*.hpp")

set_target_properties(astarte_device_sdk
    PROPERTIES
        PUBLIC_HEADER "${SDK_PUBLIC_HEADERS}"
        POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS}
)

include(GNUInstallDirs)

install(
    TARGETS astarte_device_sdk
    EXPORT astarte_device_sdk
    LIBRARY
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/astarte_device_sdk
)


# create pkg-conf .pc for astarte_device_sdk
set(PC_NAME "astarte_device_sdk")
set(PC_DESCRIPTION "Astarte Device SDK Cpp")
set(PC_VERSION ${PROJECT_VERSION})
set(PC_REQUIRES "spdlog astarte_msghub_proto grpc++ protobuf")
set(PC_REQUIRES_PRIVATE "")
set(PC_LIB "-lastarte_device_sdk -lgrpc++_reflection -laddress_sorting -lre2 -lupb --as-needed")
set(PC_LIBS_PRIVATE "")
configure_file(
    cmake/pkg-config-template.pc.in
    astarte_device_sdk.pc
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/astarte_device_sdk.pc  DESTINATION lib/pkgconfig)
