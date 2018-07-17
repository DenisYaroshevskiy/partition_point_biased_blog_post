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
using IteratorCategory = typename std::iterator_traits<I>::iterator_category;

template <typename I>
using Reference = typename std::iterator_traits<I>::reference;

template <typename I>
I middle(I f, I l) {
  static_assert(std::numeric_limits<DifferenceType<I>>::max() <=
                    std::numeric_limits<size_t>::max(),
                "iterators difference type is too big");
  return std::next(f, static_cast<std::size_t>(std::distance(f, l)) / 2);
}

template <typename I, typename P>
// requires ForwardIterator<I> && Predicate<P(ValueType<I>)>
I partition_point_n(I f, DifferenceType<I> len, P p) {
  while (len != 0) {
    DifferenceType<I> l2 = len / 2;
    I m = f;
    std::advance(m, l2);
    if (p(*m)) {
      f = ++m;
      len -= l2 + 1;
    } else
      len = l2;
  }
  return f;
}

template <typename I, typename P>
// requires ForwardIterator<I> && Predicate<P(ValueType<I>)>
I partition_point_biased_no_checks(I f, P p) {
  while (true) {
    // clang-format off
    if (!p(*f)) return f; ++f;
    if (!p(*f)) return f; ++f;
    if (!p(*f)) return f; ++f;
    // clang-format on
    for (DifferenceType<I> step = 2;; step += step) {
      I test = std::next(f, step);
      if (!p(*test)) break;
      f = ++test;
    }
  }
}

template <typename I, typename P>
// requires ForwardIterator<I> && Predicate<P(ValueType<I>)>
I partition_point_biased(I f, I l, P p, std::random_access_iterator_tag) {
  while (f != l) {
    I sent = middle(f, l);
    if (!p(*sent)) return partition_point_biased_no_checks(f, p);
    f = ++sent;
  }
  return f;
}

template <typename I, typename P>
I partition_point_biased(I f, I l, P p, std::forward_iterator_tag) {
  DifferenceType<I> step = 1;
  while (true) {
    I test = f;
    DifferenceType<I> steps_taken = 0;
    while (test != l && steps_taken != step) {
      ++test;
      ++steps_taken;
    }

    if (test == l || !p(*test)) return partition_point_n(f, steps_taken, p);

    f = ++test;
    step += step;
  }
}

template <typename I, typename P>
// requires ForwardIterator<I> && Predicate<P(ValueType<I>)>
I partition_point_biased(I f, I l, P p) {
  return partition_point_biased(f, l, p, IteratorCategory<I>{});
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