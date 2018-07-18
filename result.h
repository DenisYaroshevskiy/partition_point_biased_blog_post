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
// requires ForwardIterator<I>
void advance_checked(I& f, I l, DifferenceType<I>& n,
                     std::forward_iterator_tag) {
  while (f != l && n) {
    ++f;
    --n;
  }
}

template <typename I>
// requires RandomAccessIterator<I>
void advance_checked(I& f, I l, DifferenceType<I>& n,
                     std::random_access_iterator_tag) {
  DifferenceType<I> actual_n = std::min(n, l - f);
  f += actual_n;
  n -= actual_n;
}

template <typename I>
// requires ForwardIterator<I>
void advance_checked(I& f, I l, DifferenceType<I>& n) {
  advance_checked(f, l, n, IteratorCategory<I>{});
}

template <typename I, typename P>
// requires ForwardIterator<I> && Predicate<P(ValueType<I>)>
I partition_point_n(I f, DifferenceType<I> n, P p) {
  while (n != 0) {
    DifferenceType<I> n2 = n / 2;  // the size_t trick doesn't help here.
    I m = std::next(f, n2);
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
I lower_bound_n(I f, DifferenceType<I> n, const V& v, P p) {
  return partition_point_n(f, n, [&](Reference<I> x) { return p(x, v); });
}

template <typename I, typename V>
// requires ForwardIterator<I> && WeakComarable<ValueType<I>, V>
I lower_bound_n(I f, DifferenceType<I> n, const V& v) {
  return lower_bound_n(f, n, v, less{});
}

template <typename I, typename V, typename P>
// requires ForwardIterator<I> && StrictWeakOrder<P(ValueType<I>, V)>
I upper_bound_n(I f, DifferenceType<I> n, const V& v, P p) {
  return partition_point_n(f, n, [&](Reference<I> x) { return !p(v, x); });
}

template <typename I, typename V>
// requires ForwardIterator<I> && WeakComarable<ValueType<I>, V>
I upper_bound_n(I f, DifferenceType<I> n, const V& v) {
  return upper_bound_n(f, n, v, less{});
}

template <typename I, typename V, typename P>
// requires ForwardIterator<I> && StrictWeakOrder<P(ValueType<I>, V)>
std::pair<I, I> equal_range_n(I f, DifferenceType<I> n, const V& v, P p) {
  while (n != 0) {
    DifferenceType<I> n2 = n / 2;
    I m = std::next(f, n2);
    if (p(*m, v)) {
      f = ++m;
      n -= n2 + 1;
    } else if (p(v, *m)) {
      n = n2;
    } else {
      return {
          lower_bound_n(f, n2, v, p),
          upper_bound_n(std::next(m), n - n2 - 1, v, p),
      };
    }
  }
  return {f, f};
}

template <typename I, typename V>
// requires ForwardIterator<I> && WeakComarable<ValueType<I>, V>
std::pair<I, I> equal_range_n(I f, DifferenceType<I> n, const V& v) {
  return equal_range_n(f, n, v, less{});
}

template <typename I, typename P>
// requires ForwardIterator<I> && Predicate<P(ValueType<I>)>
I partition_point_biased_expensive_pred(I f, I l, P p) {
  DifferenceType<I> step = 1;
  while (true) {
    I test = f;
    DifferenceType<I> try_to_step = step;
    advance_checked(test, l, try_to_step);

    if (test == l || !p(*test))
      return partition_point_n(f, step - try_to_step, p);

    f = ++test;
    step += step;
  }
}

template <typename I, typename V, typename P>
// requires ForwardIterator<I> && StrictWeakOrder<P(ValueType<I>, V)>
I lower_bound_biased_expensive_cmp(I f, I l, const V& v, P p) {
  return partition_point_biased_expensive_pred(
      f, l, [&](Reference<I> x) { return p(x, v); });
}

template <typename I, typename V>
// requires ForwardIterator<I> && WeakComarable<ValueType<I>, V>
I lower_bound_biased_expensive_cmp(I f, I l, const V& v) {
  return lower_bound_biased_expensive_cmp(f, l, v, less{});
}

template <typename I, typename P>
I partition_point_biased(I f, I l, P p, std::forward_iterator_tag) {
  return partition_point_biased_expensive_pred(f, l, p);
}

template <typename I>
I middle(I f, I l) {
  static_assert(std::numeric_limits<DifferenceType<I>>::max() <=
                    std::numeric_limits<std::size_t>::max(),
                "iterators difference type is too big");
  return std::next(f, static_cast<std::size_t>(std::distance(f, l)) / 2);
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