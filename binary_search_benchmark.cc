#include <benchmark/benchmark.h>

#include <algorithm>
#include <cstdint>
#include <random>

namespace {

constexpr std::size_t kProblemSize = 1000u;

constexpr std::size_t kStep = 40;
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
    return std::find(f, l, v);
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

BENCHMARK_TEMPLATE(benchmark_search, linear)->Apply(set_looking_for_index);