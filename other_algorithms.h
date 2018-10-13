#pragma once

#include <algorithm>
#include "result.h"

#include <iostream>

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
  return v1::partition_point_biased(f, l,
                                    [&](Reference<I> x) { return p(x, v); });
}

template <typename I, typename V>
// requires ForwardIterator<I> && WeakComarable<ValueType<I>, V>
I lower_bound_biased(I f, I l, const V& v) {
  return v1::lower_bound_biased(f, l, v, less{});
}

}  // namespace v1

template <typename I, typename P>
// requires BidirectionalIterator<I> && Predicate<P(ValueType<I>)>
I partition_point_linear_with_sentinel(I f, I l, P p) {
  if (f == l) return f;

  I sent = std::prev(l);
  if (p(*sent)) return l;

  while (true) {
    if (!p(*f)) return f;
    ++f;
  }
}

template <typename I, typename V, typename P>
// requires BidirectionalIterator<I> && StrictWeakOrder<P(ValueType<I>, V)>
I lower_bound_linear_with_sentinel(I f, I l, const V& v, P p) {
  return partition_point_linear_with_sentinel(
      f, l, [&](Reference<I> x) { return p(x, v); });
}

template <typename I, typename V>
// requires BidirectionalIterator<I> && WeakComarable<ValueType<I>, V>
I lower_bound_linear_with_sentinel(I f, I l, const V& v) {
  return lower_bound_linear_with_sentinel(f, l, v, less{});
}

// Experiments for https://bugs.llvm.org/show_bug.cgi?id=39129

template <typename I, typename P>
// requires ForwardIterator<I> && Predicate<P(ValueType<I>)>
I partition_point_with_unsigned(I f, I l, P p) {
  using diff_t = std::size_t;

  diff_t n = static_cast<diff_t>(std::distance(f, l));
  while (n) {
    diff_t n2 = n / 2;
    I m = std::next(f, static_cast<DifferenceType<I>>(n2));
    if (p(*m)) {
      f = ++m;
      n -= n2 + 1;
    } else
      n = n2;
  }
  return f;
}

template <typename I, typename V, typename P>
// requires ForwardIterator<I> && StrictWeakOrder<P(ValueType<I>, V)>
I lower_bound_with_unsigned(I f, I l, const V& v, P p) {
  return partition_point_with_unsigned(f, l,
                                       [&](Reference<I> x) { return p(x, v); });
}

template <typename I, typename V>
// requires ForwardIterator<I> && Comparable<ValueType<I>, V>
I lower_bound_with_unsigned(I f, I l, const V& v) {
  return lower_bound_with_unsigned(f, l, v, less{});
}

template <typename N, typename I, typename P>
I partition_point_with_index_type_impl(I f, I l, P p, std::forward_iterator_tag) {
  return std::partition_point(f, l, p);
}

template <typename N, typename I, typename P>
// requires Numeric<N> && RandomAccessIterator<I> && Predicate<P(ValueType<I>)>
I partition_point_with_index_type_impl(I f, I l, P p, std::random_access_iterator_tag) {
  N fi = 0;
  N li = static_cast<N>(std::distance(f, l));

  while (fi != li) {
    N mi = fi + (li - fi) / 2;
    if (p(f[mi]))
      fi = ++mi;
    else
      li = mi;
  }

  return std::next(f, fi);
}

template <typename N, typename I, typename P>
// requires Numeric<N> && RandomAccessIterator<I> && Predicate<P(ValueType<I>)>
I partition_point_with_index_type(I f, I l, P p) {
  return partition_point_with_index_type_impl<N>(f, l, p, IteratorCategory<I>{});
}

template <typename N, typename I, typename V, typename P>
// requires Numeric<N> &&
//          RandomAccessIterator<I> &&
//          StrictWeakOrder<P(ValueType<I>, V)>
I lower_bound_with_index_type(I f, I l, const V& v, P p) {
  return partition_point_with_index_type<N>(
      f, l, [&](Reference<I> x) { return p(x, v); });
}

template <typename N, typename I, typename V>
// requires Numeric<N> &&
//          RandomAccessIterator<I> &&
//          Comparable<ValueType<I>, V>
I lower_bound_with_index_type(I f, I l, const V& v) {
  return lower_bound_with_index_type<N>(f, l, v, less{});
}

}  // namespace srt