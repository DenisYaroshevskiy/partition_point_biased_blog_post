#include "other_algorithms.h"
#include "third_party/catch.h"

#include <numeric>
#include <list>
#include <vector>

namespace {

template <typename I, typename Alg>
void test_lower_bound_for_range(I f, I l, Alg alg) {
  REQUIRE(alg(l, l, 10) == std::lower_bound(l, l, 10));

  I selected_f = std::prev(l);
  while(true) {
    for (int i = *selected_f - 1; i != *(std::prev(l)) + 1; ++i)
      REQUIRE(alg(selected_f, l, i) == std::lower_bound(selected_f, l, i));

    if (selected_f == f) break;
    --selected_f;
  }
}

template <typename Alg>
void test_lower_bound_biased(Alg alg) {
  std::vector<int> v_data(100u);
  std::iota(v_data.begin(), v_data.end(), 0);

  test_lower_bound_for_range(v_data.begin(), v_data.end(), alg);

  std::list<int> l_data(v_data.begin(), v_data.end());
  test_lower_bound_for_range(l_data.begin(), l_data.end(), alg);
}

}  // namespace

TEST_CASE("lower_bound_biased_v1", "[blog_post]") {
  test_lower_bound_biased([](auto f, auto l, const auto& v) {
    return srt::v1::lower_bound_biased(f, l, v);
  });
}

TEST_CASE("lower_bound_linear_with_sentinel", "[blog_post]") {
  test_lower_bound_biased([](auto f, auto l, const auto& v) {
    return srt::lower_bound_linear_with_sentinel(f, l, v);
  });
}

TEST_CASE("lower_bound_biased", "[blog_post]") {
  test_lower_bound_biased([](auto f, auto l, const auto& v) {
    return srt::lower_bound_biased(f, l, v);
  });
}
