#pragma once

#include <iterator>

namespace srt {

struct less {
  template <typename X, typename Y>
  constexpr bool operator()(const X& x, const Y& y) noexcept {
    return x < y;
  }

  using is_transparent = int;
};

template <typename I>
using DifferenceType = typename std::iterator_traits<I>::difference_type;

template <typename I>
using Reference = typename std::iterator_traits<I>::reference;

}  // namespace srt