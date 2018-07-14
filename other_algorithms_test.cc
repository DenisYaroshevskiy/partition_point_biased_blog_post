#include "other_algorithms.h"
#include "third_party/catch.h"

#include <numeric>
#include <vector>

namespace {

template <typename Alg>
void test_lower_bound_biased(Alg alg) {
  std::vector<int> data(100u);
  std::iota(data.begin(), data.end(), 0);

  REQUIRE(alg(data.end(), data.end(), 10) ==
          std::lower_bound(data.end(), data.end(), 10));

  for (auto f = --data.end();;) {
    for (int i = *f - 1; i != data.back() + 1; ++i)
      REQUIRE(alg(f, data.end(), i) == std::lower_bound(f, data.end(), i));
    if (f == data.begin()) break;
    --f;
  }
}

}  // namespace

TEST_CASE("lower_bound_biased_v1", "[blog_post]") {
  test_lower_bound_biased([](auto f, auto l, const auto& v) {
    return srt::v1::lower_bound_biased(f, l, v);
  });
}