#include <benchmark/benchmark.h>
#include "cpp_hw_2.h"

#include <vector>
#include <functional>

static void LightTransform(int& x) {
    x += 1;
}

static void HeavyTransform(int& x) {
    volatile int temp = x;
    for (int i = 0; i < 1000; ++i) {
        temp += (i % 7);
        temp *= 2;
        temp /= 2;
    }
    x = temp;
}


static void BM_ApplyFunction_Light_1Thread(benchmark::State& state) {
    const int size = static_cast<int>(state.range(0));

    for (auto _ : state) {
        std::vector<int> data(size, 1);

        ApplyFunction<int>(data, LightTransform, 1);

        benchmark::DoNotOptimize(data.data());
        benchmark::ClobberMemory();
    }
}

BENCHMARK(BM_ApplyFunction_Light_1Thread)
    ->Arg(100)
    ->Arg(1000)
    ->Arg(10000)
    ->Arg(100000);


static void BM_ApplyFunction_Light_4Threads(benchmark::State& state) {
    const int size = static_cast<int>(state.range(0));

    for (auto _ : state) {
        std::vector<int> data(size, 1);

        ApplyFunction<int>(data, LightTransform, 4);

        benchmark::DoNotOptimize(data.data());
        benchmark::ClobberMemory();
    }
}

BENCHMARK(BM_ApplyFunction_Light_4Threads)
    ->Arg(100)
    ->Arg(1000)
    ->Arg(10000)
    ->Arg(100000);


static void BM_ApplyFunction_Heavy_1Thread(benchmark::State& state) {
    const int size = static_cast<int>(state.range(0));

    for (auto _ : state) {
        std::vector<int> data(size, 1);

        ApplyFunction<int>(data, HeavyTransform, 1);

        benchmark::DoNotOptimize(data.data());
        benchmark::ClobberMemory();
    }
}

BENCHMARK(BM_ApplyFunction_Heavy_1Thread)
    ->Arg(100)
    ->Arg(1000)
    ->Arg(10000);


static void BM_ApplyFunction_Heavy_4Threads(benchmark::State& state) {
    const int size = static_cast<int>(state.range(0));

    for (auto _ : state) {
        std::vector<int> data(size, 1);

        ApplyFunction<int>(data, HeavyTransform, 4);

        benchmark::DoNotOptimize(data.data());
        benchmark::ClobberMemory();
    }
}

BENCHMARK(BM_ApplyFunction_Heavy_4Threads)
    ->Arg(100)
    ->Arg(1000)
    ->Arg(10000);

BENCHMARK_MAIN();
