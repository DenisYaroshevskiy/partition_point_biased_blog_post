#include <benchmark/benchmark.h>

#include <algorithm>
#include <cstdint>
#include <random>

#include "other_algorithms.h"

namespace {

constexpr std::size_t kProblemSize = 1000u;
constexpr std::size_t kStep = 1;
constexpr std::size_t kMaxIdx = kProblemSize;


const std::vector<std::int64_t>& ints_test() {
  static const auto res = [] {
    std::mt19937 g;
    std::uniform_int_distribution<std::int64_t> dis(
        1, static_cast<std::int64_t>(kProblemSize) * 10);

    std::vector<std::int64_t> v(kProblemSize);
    std::generate(v.begin(), v.end(), [&] { return dis(g); });
    std::sort(v.begin(), v.end());
    return v;
  }();

  return res;
}

void set_looking_for_index(benchmark::internal::Benchmark* bench) {
  for (std::size_t looking_for_idx = 0; looking_for_idx < kMaxIdx; looking_for_idx += kStep)
    bench->Arg(static_cast<int>(looking_for_idx));
}

struct linear {
  template <typename I, typename V>
  I operator()(I f, I l, const V& v) {
    return std::find_if(f, l, [&](const auto& x) { return !(x < v); });
  }
};

struct binary {
  template <typename I, typename V>
  I operator()(I f, I l, const V& v) {
    return std::lower_bound(f, l, v);
  }
};

struct biased_v1 {
  template <typename I, typename V>
  I operator()(I f, I l, const V& v) {
    return srt::v1::lower_bound_biased(f, l, v);
  }
};

struct linear_with_sentinel {
  template <typename I, typename V>
  I operator()(I f, I l, const V& v) {
    return srt::lower_bound_linear_with_sentinel(f, l, v);
  }
};

struct biased_final {
  template <typename I, typename V>
  I operator()(I f, I l, const V& v) {
    return srt::lower_bound_biased(f, l, v);
  }
};

}  // namespace

template <typename Searcher>
void benchmark_search(benchmark::State& state) {
  auto input = ints_test();
  auto looking_for = input[static_cast<std::size_t>(state.range(0))];
  for (auto _ : state)
    benchmark::DoNotOptimize(Searcher{}(input.begin(), input.end(), looking_for));
}

BENCHMARK_TEMPLATE(benchmark_search, biased_final)->Apply(set_looking_for_index);