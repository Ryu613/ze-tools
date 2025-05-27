#include "ze/collection/sparse_set.hpp"
#include "ze/collection/pmr_sparse_set.hpp"

#include <random>
#include <chrono>
#include <algorithm>
#include <vector>
#include <iostream>
#include <unordered_set>
#include <array>

using namespace ze::collection;

constexpr size_t N = 1'000'000;
constexpr size_t TEST_RUNS = 5;

std::vector<uint32_t> generate_random_input(size_t n, uint32_t max_value) {
    std::mt19937 rng(42);
    std::uniform_int_distribution<uint32_t> dist(0, max_value);
    std::vector<uint32_t> data(n);
    std::generate(data.begin(), data.end(), [&]() { return dist(rng); });
    return data;
}

template <typename Func>
long long benchmark(Func func, const std::string& label) {
    auto start = std::chrono::steady_clock::now();
    func();
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << label << ": " << duration << " ms" << std::endl;
    return duration;
}

int main() {
    auto data = generate_random_input(N, N * 2);
    auto buffer = std::make_unique<std::byte[]>(64 * 1024 * 1024);

    for (size_t i = 0; i < TEST_RUNS; ++i) {
        uint32_t max_x = *std::max_element(data.begin(), data.end());
        benchmark([&]() {
            sparse_set<uint32_t> s(max_x + 1);
            // 真实场景模拟：插入前 N/2，查找 N 次，删除 N/4
            for (size_t i = 0; i < N / 2; ++i) s.insert(data[i]);
            for (size_t i = 0; i < N; ++i) s.contains(data[i]);
            for (size_t i = 0; i < N / 4; ++i) s.erase(data[i]);
            }, "sparse_set run " + std::to_string(i + 1));

        benchmark([&]() {
            std::pmr::unsynchronized_pool_resource  pool;
            pmr::sparse_set<uint32_t> s(64, &pool);
            for (auto x : data) s.insert(x);
            for (auto x : data) s.contains(x);
            for (auto x : data) s.erase(x);
            }, "pmr_sparse_set run " + std::to_string(i + 1));

        benchmark([&]() {
            std::unordered_set<uint32_t> s(max_x + 1);
            for (size_t i = 0; i < N / 2; ++i) s.insert(i);
            for (size_t i = 0; i < N; ++i) {
                [[maybe_unused]] auto it = s.find(i);
            }
            for (size_t i = 0; i < N / 4; ++i) s.erase(i);
            }, "unordered_set run " + std::to_string(i + 1));

        std::cout << "------------------------------" << std::endl;
    }

    return 0;
}