#include <iostream>
#include <atomic>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <thread>
#include <vector>

inline void FutexWait(void* value, int expectedValue) {
    syscall(SYS_futex, value, FUTEX_WAIT_PRIVATE, expectedValue, nullptr, nullptr, 0);
}

inline void FutexWake(void* value, int count) {
    syscall(SYS_futex, value, FUTEX_WAKE_PRIVATE, count, nullptr, nullptr, 0);
}

class FutexMutex {
public:
    FutexMutex() = default;

    FutexMutex(const FutexMutex&) = delete;
    FutexMutex& operator=(const FutexMutex&) = delete;

    void lock() {
        while (true) {
            int expected = 0;
            if (state_.compare_exchange_weak(expected, 1, std::memory_order_acquire)) {
                return;
            }

            FutexWait(&state_, 1);
        }
    }

    void unlock() {
        state_.store(0, std::memory_order_release);
        FutexWake(&state_, 1);
    }

private:
    std::atomic<int> state_{0};
};

int main() {
    FutexMutex mutex;

    int kThreads = 8;
    int kIterations = 1000;

    int counter = 0;

    std::vector<std::thread> threads;

    for (int i = 0; i < kThreads; ++i) {
        threads.emplace_back([&]() {
            for (int j = 0; j < kIterations; ++j) {
                mutex.lock();
                ++counter;
                mutex.unlock();
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Expected: " << kThreads * kIterations << std::endl;
    std::cout << "Actual:   " << counter << std::endl;

    if (counter == kThreads * kIterations) {
        std::cout << "OK\n";
    } else {
        std::cout << "ERROR\n";
    }

    return 0;
}
