#include <functional>
#include <map>
#include <random>
#include <set>
#include <vector>

#include <iostream>

#include "third_party/catch.h"
#include "result.h"
#include <boost/container/flat_set.hpp>
#include <boost/container/flat_map.hpp>

using boost::container::flat_map;
using boost::container::flat_set;

template <typename K, typename V>
flat_map<K, flat_set<V>> build_flat_map_of_flat_sets(std::vector<std::pair<K, V>> buf) {
  std::sort(buf.begin(), buf.end());
  buf.erase(std::unique(buf.begin(), buf.end()), buf.end());

  flat_map<K, flat_set<V>> res;
  for (auto r : srt::group_equals(buf.begin(), buf.end(),
                                  [](const auto& x, const auto& y) { return x.first < y.first; })) {
    res.emplace_hint(res.end(), std::move(r.begin()->first), flat_set<V>{});
    auto& cur_set = (--res.end())->second;
    for (auto& elem : r)
      cur_set.insert(cur_set.end(), std::move(elem.second));
  }

  return res;
}

template <typename C>
std::vector<std::pair<int, int>> to_vector_of_pairs_for_test(const C& c) {
  std::vector<std::pair<int, int>> res;
  for (const auto& pr : c) {
    for (int v : pr.second)
      res.emplace_back(pr.first, v);
  }
  return res;
}

TEST_CASE("build_flat_map_of_flat_sets", "[usage_examples]") {
  const std::vector<std::pair<int, int>> input = [] {
    std::uniform_int_distribution<> dist(0, 100);
    std::mt19937 g;

    std::vector<std::pair<int, int>> res;

    for (int i = 0; i < 100; ++i) {
      res.emplace_back(dist(g), dist(g));
    }

    return res;
  }();

  for (auto f = input.begin(); f != input.end(); ++f)
    for (auto l = f; l != input.end(); ++l) {
      std::map<int, std::set<int>> expected;
      for (auto i = f; i != l; ++i)
        expected[i->first].insert(i->second);

      auto actual = build_flat_map_of_flat_sets(std::vector<std::pair<int, int>>{f, l});

      auto expected_vec = to_vector_of_pairs_for_test(expected);
      auto actual_vec = to_vector_of_pairs_for_test(actual);

      REQUIRE(expected_vec.size() == actual_vec.size());
      REQUIRE(expected_vec == actual_vec);
    }
}
