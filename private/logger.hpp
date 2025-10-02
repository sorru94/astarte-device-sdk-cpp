// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef LOGGER_H
#define LOGGER_H

#if __has_include("spdlog/spdlog.h")
#include "spdlog/spdlog.h"
#elif __has_include(<format>)
#include <format>
#include <iostream>
#include <string>
#include <string_view>
#else
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#endif

namespace AstarteDeviceSdk {
namespace logging {

#if __has_include("spdlog/spdlog.h")
template <typename... Args>
void trace(spdlog::format_string_t<Args...> fmt, Args&&... args) {
  spdlog::trace(fmt, std::forward<Args>(args)...);
}
template <typename... Args>
void debug(spdlog::format_string_t<Args...> fmt, Args&&... args) {
  spdlog::debug(fmt, std::forward<Args>(args)...);
}
template <typename... Args>
void info(spdlog::format_string_t<Args...> fmt, Args&&... args) {
  spdlog::info(fmt, std::forward<Args>(args)...);
}
template <typename... Args>
void warn(spdlog::format_string_t<Args...> fmt, Args&&... args) {
  spdlog::warn(fmt, std::forward<Args>(args)...);
}
template <typename... Args>
void error(spdlog::format_string_t<Args...> fmt, Args&&... args) {
  spdlog::error(fmt, std::forward<Args>(args)...);
}
template <typename... Args>
void critical(spdlog::format_string_t<Args...> fmt, Args&&... args) {
  spdlog::critical(fmt, std::forward<Args>(args)...);
}
#elif __has_include(<format>)
template <typename... Args>
void trace(const char* fmt, Args&&... args) {
  // Ignore the trace level to avoid polluting the stdout
}
template <typename... Args>
void debug(const char* fmt, Args&&... args) {
  // Ignore the debug level to avoid polluting the stdout
}
template <typename... Args>
void info(std::format_string<Args...> fmt, Args&&... args) {
  std::cout << "[info] " << std::format(fmt, std::forward<Args>(args)...) << std::endl;
}
inline void info(std::string_view msg) { std::cout << "[info] " << msg << std::endl; }
template <typename... Args>
void warn(std::format_string<Args...> fmt, Args&&... args) {
  std::cout << "[warn] " << std::format(fmt, std::forward<Args>(args)...) << std::endl;
}
inline void warn(std::string_view msg) { std::cerr << "[warn] " << msg << std::endl; }
template <typename... Args>
void error(std::format_string<Args...> fmt, Args&&... args) {
  std::cout << "[error] " << std::format(fmt, std::forward<Args>(args)...) << std::endl;
}
inline void error(std::string_view msg) { std::cerr << "[error] " << msg << std::endl; }
template <typename... Args>
void critical(std::format_string<Args...> fmt, Args&&... args) {
  std::cout << "[critical] " << std::format(fmt, std::forward<Args>(args)...) << std::endl;
}
inline void critical(std::string_view msg) { std::cerr << "[critical] " << msg << std::endl; }
#else
template <typename... Args>
void trace(const char* /*fmt*/, Args&&... /*args*/) {
  // Ignore the trace level to avoid polluting the stdout
}
template <typename... Args>
void debug(const char* /*fmt*/, Args&&... /*args*/) {
  // Ignore the debug level to avoid polluting the stdout
}
template <typename... Args>
void info(const char* fmt, Args&&... args) {
  std::cout << "[info] " << fmt << std::endl;
}
inline void info(std::string_view msg) { std::cout << "[info] " << msg << std::endl; }
template <typename... Args>
void warn(const char* fmt, Args&&... args) {
  std::cout << "[warn] " << fmt << std::endl;
}
inline void warn(std::string_view msg) { std::cerr << "[warn] " << msg << std::endl; }
template <typename... Args>
void error(const char* fmt, Args&&... args) {
  std::cout << "[error] " << fmt << std::endl;
}
inline void error(std::string_view msg) { std::cerr << "[error] " << msg << std::endl; }
template <typename... Args>
void critical(const char* fmt, Args&&... args) {
  std::cout << "[critical] " << fmt << std::endl;
}
inline void critical(std::string_view msg) { std::cerr << "[critical] " << msg << std::endl; }
#endif

}  // namespace logging
}  // namespace AstarteDeviceSdk

#endif  // LOGGER_H
