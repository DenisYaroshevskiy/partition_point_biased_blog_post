#pragma once

#include <algorithm>
#include "result.h"

namespace srt {

namespace v1 {

template <typename I, typename P>
// requires ForwardIterator<I> && Predicate<P(ValueType<I>)>
I partition_point_biased(I f, I l, P p) {
  DifferenceType<I> length = std::distance(f, l);
  DifferenceType<I> step = 1;

  while (length > step) {
    I test = std::next(f, step);
    if (!p(*test)) {
      l = test;
      break;
    }
    f = ++test;
    length -= step + 1;
    step += step;
  }

  return std::partition_point(f, l, p);
}

template <typename I, typename V, typename P>
// requires ForwardIterator<I> && StrictWeakOrder<P(ValueType<I>, V)>
I lower_bound_biased(I f, I l, const V& v, P p) {
  return v1::partition_point_biased(f, l, [&](Reference<I> x) { return p(x, v); });
}

template <typename I, typename V>
// requires ForwardIterator<I> && WeakComarable<ValueType<I>, V>
I lower_bound_biased(I f, I l, const V& v) {
  return v1::lower_bound_biased(f, l, v, less{});
}

}  // namespace v1

template <typename I, typename P>
// requires ForwardIterator<I> && Predicate<P(ValueType<I>)>
I partition_point_linear_with_sentinel(I f, I l, P p) {
  if (f == l) return f;

  I sent = std::prev(l);
  if (p(*sent)) return sent;

  while (true) {
    if (!p(*f)) return f;
    ++f;
  }
}

template <typename I, typename V, typename P>
// requires ForwardIterator<I> && StrictWeakOrder<P(ValueType<I>, V)>
I lower_bound_linear_with_sentinel(I f, I l, const V& v, P p) {
  return partition_point_linear_with_sentinel(
      f, l, [&](Reference<I> x) { return p(x, v); });
}

template <typename I, typename V>
// requires ForwardIterator<I> && WeakComarable<ValueType<I>, V>
I lower_bound_linear_with_sentinel(I f, I l, const V& v) {
  return lower_bound_linear_with_sentinel(f, l, v, less{});
}

}  // namespace srt