#include <benchmark/benchmark.h>

#include <algorithm>

struct linear_searcher {
  template <typename I, typename V>
  I operator(I f, I l, const V& v) {
    return std::find(f, l, v);
  }
};