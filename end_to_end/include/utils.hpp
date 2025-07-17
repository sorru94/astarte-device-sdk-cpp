// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <list>
#include <sstream>
#include <string>

template <typename T>
auto format_list(const std::list<T>& input) -> std::string {
  std::ostringstream oss;
  oss << "{";
  for (auto v : input) {
    oss << v.format() << ", ";
  }
  oss << "}";
  return oss.str();
}

template <typename T>
bool compare_lists(const std::list<T>& list1, const std::list<T>& list2) {
  if (list1.size() != list2.size()) {
    return false;
  }

  auto temp_list2 = list2;
  for (const auto& item1 : list1) {
    auto it = std::find(temp_list2.begin(), temp_list2.end(), item1);
    if (it != temp_list2.end()) {
      temp_list2.erase(it);
    } else {
      return false;
    }
  }

  return true;
}
