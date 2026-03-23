#pragma once

#include <vector>
#include <functional>
#include <thread>
#include <algorithm>

template <typename T>
void ApplyFunction(std::vector<T>& data,
                   const std::function<void(T&)>& transform,
                   const int threadCount = 1) {
    if (data.empty()) {
        return;
    }

    int actualThreadCount = std::max(1, threadCount);
    actualThreadCount = std::min(actualThreadCount, static_cast<int>(data.size()));

    std::vector<std::thread> threads;
    threads.reserve(actualThreadCount);

    std::size_t blockSize = data.size() / actualThreadCount;
    std::size_t begin = 0;

    for (int i = 0; i < actualThreadCount; ++i) {
        std::size_t end = (i == actualThreadCount - 1) ? data.size() : begin + blockSize;

        threads.emplace_back([begin, end, &data, &transform]() {
            for (std::size_t j = begin; j < end; ++j) {
                transform(data[j]);
            }
        });

        begin = end;
    }

    for (auto& thread : threads) {
        thread.join();
    }
}
