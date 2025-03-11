// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <mutex>
#include <optional>
#include <queue>

template <typename T>
class SharedQueue {
 public:
  auto pop() -> std::optional<T> {
    std::unique_lock<std::mutex> mlock(mutex_);
    std::optional<T> res = std::nullopt;
    if (!queue_.empty()) {
      res = queue_.front();
      queue_.pop();
    }
    return res;
  }
  void push(const T& item) {
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push(item);
  }
  auto size() -> std::size_t {
    std::unique_lock<std::mutex> mlock(mutex_);
    const std::size_t size = queue_.size();
    return size;
  }
  auto empty() -> bool {
    std::unique_lock<std::mutex> mlock(mutex_);
    const bool res = queue_.empty();
    return res;
  }

 private:
  std::queue<T> queue_;
  std::mutex mutex_;
};
