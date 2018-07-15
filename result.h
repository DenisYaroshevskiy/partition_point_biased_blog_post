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

template <typename I>
I middle(I f, I l) {
  static_assert(
    std::numeric_limits<DifferenceType<I>>::max() <=
    std::numeric_limits<size_t>::max(),
    "iterators difference type is too big");
  return std::next(f, static_cast<std::size_t>(std::distance(f, l)) / 2);
}

template <typename I, typename P>
// requires ForwardIterator<I> && Predicate<P(ValueType<I>)>
I partition_point_biased_no_checks(I f, P p) {
  while(true) {
    if (!p(*f)) return f; ++f;
    if (!p(*f)) return f; ++f;
    if (!p(*f)) return f; ++f;
    for (DifferenceType<I> step = 2;; step += step) {
      I test = std::next(f, step);
      if (!p(*test)) break;
      f = ++test;
    }
  }
}

template <typename I, typename P>
// requires ForwardIterator<I> && Predicate<P(ValueType<I>)>
I partition_point_biased(I f, I l, P p) {
  while (f != l) {
    I sent = middle(f, l);
    if (!p(*sent)) return partition_point_biased_no_checks(f, p);
    f = ++sent;
  }
  return f;
}

template <typename I, typename V, typename P>
// requires ForwardIterator<I> && StrictWeakOrder<P(ValueType<I>, V)>
I lower_bound_biased(I f, I l, const V& v, P p) {
  return partition_point_biased(f, l, [&](Reference<I> x) { return p(x, v); });
}

template <typename I, typename V>
// requires ForwardIterator<I> && WeakComarable<ValueType<I>, V>
I lower_bound_biased(I f, I l, const V& v) {
  return lower_bound_biased(f, l, v, less{});
}

}  // namespace srt