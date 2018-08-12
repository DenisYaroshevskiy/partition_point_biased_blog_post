#include <algorithm>
#include <cstdint>
#include <iostream>
#include <random>
#include <set>
#include <string>

#include "other_algorithms.h"

template <typename I, typename Alg>
int count_compare_invocations(I f, I l, int pos, Alg alg) {
  int res = 0;
  alg(f, l, f[pos], [&](int x, int y) mutable {
    ++res;
    return x < y;
  });
  return res;
}

template <typename I, typename Alg>
void mimicking_gbench_output(I f, I l, int pos, Alg alg) {
  int res = count_compare_invocations(f, l, pos, alg);
// clang-format off
  std::cout
    <<"{\n" <<                                                                           //
           "\"name\" : \"benchmark_search<" << alg.name() << ">/" << pos << "\",\n" <<   //
           "\"real_time\" : " << res <<  //
      "\n}";
// clang-format on
}

struct binary {
  template <typename I, typename V, typename P>
  I operator()(I f, I l, const V& v, P p) {
    return std::lower_bound(f, l, v, p);
  }

  std::string name() const { return "binary"; }
};

struct biased_final {
  template <typename I, typename V, typename P>
  I operator()(I f, I l, const V& v, P p) {
    return srt::lower_bound_biased(f, l, v, p);
  }

  std::string name() const { return "biased_final"; }
};

struct biased_expensive_cmp {
  template <typename I, typename V, typename P>
  I operator()(I f, I l, const V& v, P p) {
    return srt::lower_bound_biased_expensive_cmp(f, l, v, p);
  }

  std::string name() const { return "biased_expensive_cmp"; }
};

int main() {
  constexpr std::size_t kProblemSize = 1000u;

  std::mt19937 g;
  std::uniform_int_distribution<std::int64_t> dis(
      1, static_cast<std::int64_t>(kProblemSize) * 10);

  std::set<std::int64_t> unique_sorted_ints;
  while (unique_sorted_ints.size() < kProblemSize)
    unique_sorted_ints.insert(dis(g));

  std::vector<std::int64_t> v(unique_sorted_ints.begin(),
                              unique_sorted_ints.end());

  std::cout << "{\n\"benchmarks\": [\n";
  for (int i = 0; i < static_cast<int>(kProblemSize); ++i) {
    if (i != 0) std::cout << ",\n";
    mimicking_gbench_output(&v[0], &v[0] + v.size(), i, biased_expensive_cmp{});
  }
  std::cout << "]}" << std::endl;
}
