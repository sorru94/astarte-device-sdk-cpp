# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.15)

project(
  AstarteDeviceSDKcpp
  VERSION 1.0
  LANGUAGES CXX)

include(FetchContent)

option(USE_SYSTEM_GRPC "Use system installed gRPC" OFF)
if(USE_SYSTEM_GRPC)
  # Protobuf
  set(protobuf_MODULE_COMPATIBLE TRUE)
  find_package(Protobuf CONFIG REQUIRED)
  message(STATUS "Using protobuf ${protobuf_VERSION}")
  set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)

  # gRPC
  find_package(gRPC CONFIG REQUIRED)
  message(STATUS "Using gRPC ${gRPC_VERSION}")
  set(_GRPC_GRPCPP gRPC::grpc++)
  set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
else()
  if(MSVC)
    add_definitions(-D_WIN32_WINNT=0x600)
  endif()

  find_package(Threads REQUIRED)

  set(ABSL_ENABLE_INSTALL ON)
  set(ABSL_PROPAGATE_CXX_STD ON)

  message(STATUS "Using gRPC via add_subdirectory (FetchContent).")
  FetchContent_Declare(
    grpc
    GIT_REPOSITORY https://github.com/grpc/grpc.git
    GIT_TAG        v1.69.0)
  FetchContent_MakeAvailable(grpc)

  # Since FetchContent uses add_subdirectory under the hood, we can use
  # the grpc targets directly from this build.
  set(_PROTOBUF_LIBPROTOBUF libprotobuf)
  set(_REFLECTION grpc++_reflection)
  set(_PROTOBUF_PROTOC $<TARGET_FILE:protoc>)
  set(_GRPC_GRPCPP grpc++)
  set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)
endif()

# Logging library
set(SPDLOG_GITHUB_URL https://github.com/gabime/spdlog.git)
set(SPDLOG_REPO_BRANCH v1.15.1)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY ${SPDLOG_GITHUB_URL}
  GIT_TAG        ${SPDLOG_REPO_BRANCH}
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
    base64
    GIT_REPOSITORY https://github.com/aklomp/base64.git
    GIT_TAG        v0.5.2
)
FetchContent_MakeAvailable(base64)

# Astarte message hub protos
set(MSGHUB_PROTO_REPO_URL https://github.com/astarte-platform/astarte-message-hub-proto.git)
set(MSGHUB_PROTO_REPO_BRANCH 87e9b3937cd563576de4bcb801a91c700bf196bb)
FetchContent_Declare(
    astarte_msghub_proto
    GIT_REPOSITORY ${MSGHUB_PROTO_REPO_URL}
    GIT_TAG        ${MSGHUB_PROTO_REPO_BRANCH}
)
FetchContent_MakeAvailable(astarte_msghub_proto)

# Create a library from the source code
add_library(astarte_device_sdk)
target_include_directories(astarte_device_sdk PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(astarte_device_sdk PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/private)
file(GLOB astarte_sdk_src "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
target_sources(astarte_device_sdk PRIVATE ${astarte_sdk_src})

# Add the proto sources and headers
file(GLOB astarte_msghub_proto_src "${astarte_msghub_proto_SOURCE_DIR}/cpp/astarteplatform/msghub/*.cc")
if(MAKE_DEPENDENCIES_PUBLIC)
target_include_directories(astarte_device_sdk PUBLIC "${astarte_msghub_proto_SOURCE_DIR}/cpp")
else()
target_include_directories(astarte_device_sdk PRIVATE "${astarte_msghub_proto_SOURCE_DIR}/cpp")
endif()
target_sources(astarte_device_sdk PRIVATE ${astarte_msghub_proto_src})

# Link the dependencies within the library
option(MAKE_DEPENDENCIES_PUBLIC "Make installed dependencies public" OFF)
if(MAKE_DEPENDENCIES_PUBLIC)
target_link_libraries(astarte_device_sdk
  PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>
  PRIVATE base64
  PUBLIC absl::check
  PUBLIC ${_REFLECTION}
  PUBLIC ${_GRPC_GRPCPP}
  PUBLIC ${_PROTOBUF_LIBPROTOBUF})
else()
target_link_libraries(astarte_device_sdk
  PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>
  PRIVATE base64
  PRIVATE absl::check
  PRIVATE ${_REFLECTION}
  PRIVATE ${_GRPC_GRPCPP}
  PRIVATE ${_PROTOBUF_LIBPROTOBUF})
endif()
