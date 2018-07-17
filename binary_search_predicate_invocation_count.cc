#include <algorithm>
#include <cstdint>
#include <iostream>
#include <random>
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
  std::cout << "{\n"
            <<  //
      "\"name\" : \"benchmark_search<" << alg.name() << ">/" << pos << "\",\n"
            <<                      //
      "\"real_time\" : " << res <<  //
      "\n}";
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

struct biased_forward {
  template <typename I, typename V, typename P>
  I operator()(I f, I l, const V& v, P p) {
    return srt::partition_point_biased(
        f, l, [&](srt::Reference<I> x) { return p(x, v); },
        std::forward_iterator_tag{});
  }

  std::string name() const { return "biased_forward"; }
};

int main() {
  constexpr std::size_t kProblemSize = 1000u;

  std::mt19937 g;
  std::uniform_int_distribution<std::int64_t> dis(
      1, static_cast<std::int64_t>(kProblemSize) * 10);

  std::vector<std::int64_t> v(kProblemSize);
  std::generate(v.begin(), v.end(), [&] { return dis(g); });
  std::sort(v.begin(), v.end());

  std::cout << "{\n\"benchmarks\": [\n";
  for (int i = 0; i < static_cast<int>(kProblemSize); ++i) {
    if (i != 0) std::cout << ",\n";
    mimicking_gbench_output(&v[0], &v[0] + v.size(), i, biased_forward{});
  }
  std::cout << "]}" << std::endl;
}
