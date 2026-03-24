#include <condition_variable>
#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

template <typename T>
class SharedState {
public:
    void SetValue(T value) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            value_ = std::move(value);
            ready_ = true;
        }
        cv_.notify_all();
    }

    void SetException(std::exception_ptr exception) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            exception_ = exception;
            ready_ = true;
        }
        cv_.notify_all();
    }

    void Wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]() {
            return ready_;
        });
    }

    T Get() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]() {
            return ready_;
        });
    
        if (exception_) {
            std::rethrow_exception(exception_);
        }
    
        return *value_;
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    bool ready_ = false;
    std::optional<T> value_;
    std::exception_ptr exception_;
};

template <typename T>
class Future {
public:
    explicit Future(std::shared_ptr<SharedState<T>> state)
        : state_(std::move(state)) {}

    void Wait() {
        state_->Wait();
    }

    T Get() {
        return state_->Get();
    }

private:
    std::shared_ptr<SharedState<T>> state_;
};

class ThreadPool {
public:
    explicit ThreadPool(size_t thread_count) {
        if (thread_count == 0) {
            throw std::invalid_argument("thread_count must be positive");
        }

        for (size_t i = 0; i < thread_count; ++i) {
            workers_.emplace_back([this]() {
                WorkerLoop();
            });
        }
    }

    ~ThreadPool() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stop_ = true;
        }

        cv_.notify_all();

        for (auto& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    template <typename T>
    Future<T> Submit(std::function<T()> func) {
        auto state = std::make_shared<SharedState<T>>();

        auto wrapper = [func, state]() {
            try {
                T result = func();
                state->SetValue(result);
            } catch (...) {
                state->SetException(std::current_exception());
            }
        };

        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (stop_) {
                throw std::runtime_error("ThreadPool is stopped");
            }
            tasks_.push(std::move(wrapper));
        }

        cv_.notify_one();
        return Future<T>(state);
    }

private:
    void WorkerLoop() {
        while (true) {
            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, [this]() {
                    return stop_ || !tasks_.empty();
                });

                if (stop_ && tasks_.empty()) {
                    return;
                }

                task = std::move(tasks_.front());
                tasks_.pop();
            }

            task();
        }
    }

private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool stop_ = false;
};


int main() {
    // ThreadPool pool(1);
    ThreadPool pool(2);

    auto future1 = pool.Submit<int>([]() {
        int count = 0;
        while (count < 100000) {
            count++;
        };
        return count;
    });

    auto future2 = pool.Submit<int>([]() {
        int sum = 0;
        for (int i = 1; i <= 5; ++i) {
            sum += i;
        }
        return sum;
    });

    std::cout << "future1 result = " << future1.Get() << '\n';
    std::cout << "future2 result = " << future2.Get() << '\n';

    return 0;
}
