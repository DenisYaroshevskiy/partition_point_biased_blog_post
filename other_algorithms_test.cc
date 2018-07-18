#include "other_algorithms.h"
#include "third_party/catch.h"

#include <list>
#include <numeric>
#include <vector>

namespace {

template <typename I, typename Test>
void run_for_inputs(I f, I l, Test test) {
  test(l, l, 10);

  I selected_f = std::prev(l);
  while (true) {
    for (int i = *selected_f - 1; i != *(std::prev(l)) + 1; ++i)
      test(selected_f, l, i);

    if (selected_f == f) break;
    --selected_f;
  }
}

template <typename Alg>
void test_lower_bound(Alg alg) {
  std::vector<int> v_data(100u);
  std::iota(v_data.begin(), v_data.end(), 0);

  auto test = [&](auto f, auto l, int v) {
    REQUIRE(alg(f, l, v) == std::lower_bound(f, l, v));
  };

  run_for_inputs(v_data.begin(), v_data.end(), test);

  std::list<int> l_data(v_data.begin(), v_data.end());
  run_for_inputs(l_data.begin(), l_data.end(), test);
}

template <typename Alg>
void test_upper_bound(Alg alg) {
  std::vector<int> v_data;
  for (int i = 0; i < 10; ++i)
    for (int j = 0; j < i; ++j)
      v_data.push_back(i);

  auto test = [&](auto f, auto l, int v) {
    REQUIRE(alg(f, l, v) == std::upper_bound(f, l, v));
  };

  run_for_inputs(v_data.begin(), v_data.end(), test);

  std::list<int> l_data(v_data.begin(), v_data.end());
  run_for_inputs(l_data.begin(), l_data.end(), test);
}

template <typename Alg>
void test_equal_range(Alg alg) {
  std::vector<int> v_data;
  for (int i = 0; i < 10; ++i)
    for (int j = 0; j < i; ++j)
      v_data.push_back(i);

  auto test = [&](auto f, auto l, int v) {
    auto alg_res = alg(f, l, v);
    auto std_res = std::equal_range(f, l, v);
    std::vector<int> alg_as_vec{alg_res.first, alg_res.second};
    std::vector<int> std_as_vec{std_res.first, std_res.second};

    REQUIRE(alg_as_vec == std_as_vec);
  };

  run_for_inputs(v_data.begin(), v_data.end(), test);

  std::list<int> l_data(v_data.begin(), v_data.end());
  run_for_inputs(l_data.begin(), l_data.end(), test);
}

}  // namespace

TEST_CASE("lower_bound_biased_v1", "[blog_post]") {
  test_lower_bound([](auto f, auto l, const auto& v) {
    return srt::v1::lower_bound_biased(f, l, v);
  });
}

TEST_CASE("lower_bound_linear_with_sentinel", "[blog_post]") {
  test_lower_bound([](auto f, auto l, const auto& v) {
    return srt::lower_bound_linear_with_sentinel(f, l, v);
  });
}

TEST_CASE("lower_bound_n", "[blog_post]") {
  test_lower_bound([](auto f, auto l, const auto& v) {
    return srt::lower_bound_n(f, std::distance(f, l), v);
  });
}

TEST_CASE("lower_bound_biased", "[blog_post]") {
  test_lower_bound([](auto f, auto l, const auto& v) {
    return srt::lower_bound_biased(f, l, v);
  });
}

TEST_CASE("upper_bound_n", "[blog_post]") {
  test_upper_bound([](auto f, auto l, const auto& v) {
    return srt::upper_bound_n(f, std::distance(f, l), v);
  });
}

TEST_CASE("equal_range_n", "[blog_post]") {
  test_equal_range([](auto f, auto l, const auto& v) {
    return srt::equal_range_n(f, std::distance(f, l), v);
  });
}