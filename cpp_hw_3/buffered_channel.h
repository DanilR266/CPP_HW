#pragma once

#include <optional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdexcept>

template <class T>
class BufferedChannel {
public:
    explicit BufferedChannel(int size)
        : capacity_(size), closed_(false) {
        if (size <= 0) {
            throw std::invalid_argument("BufferedChannel size must be positive");
        }
    }

    void Send(const T& value) {
        std::unique_lock<std::mutex> lock(mutex_);

        not_full_cv_.wait(lock, [this]() {
            return closed_ || queue_.size() < capacity_;
        });

        if (closed_) {
            throw std::runtime_error("Send to closed channel");
        }

        queue_.push(value);

        not_empty_cv_.notify_one();
    }

    std::optional<T> Recv() {
        std::unique_lock<std::mutex> lock(mutex_);

        not_empty_cv_.wait(lock, [this]() {
            return closed_ || !queue_.empty();
        });

        if (queue_.empty()) {
            return std::nullopt;
        }

        T value = std::move(queue_.front());
        queue_.pop();

        not_full_cv_.notify_one();

        return value;
    }

    void Close() {
        std::lock_guard<std::mutex> lock(mutex_);

        if (closed_) {
            return;
        }

        closed_ = true;

        not_full_cv_.notify_all();
        not_empty_cv_.notify_all();
    }

private:
    std::size_t capacity_;
    std::queue<T> queue_;
    bool closed_;

    std::mutex mutex_;
    std::condition_variable not_full_cv_;
    std::condition_variable not_empty_cv_;
};
