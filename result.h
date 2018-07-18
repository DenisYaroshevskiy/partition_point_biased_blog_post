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
constexpr bool is_bidirectional_v =
    std::is_base_of<std::bidirectional_iterator_tag,
                    IteratorCategory<I>>::value;

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

template <typename I, typename V, typename P>
// requires ForwardIterator<I> && StrictWeakOrder<P(ValueType<I>, V)>
I upper_bound_biased_expensive_cmp(I f, I l, const V& v, P p) {
  return partition_point_biased_expensive_pred(
      f, l, [&](Reference<I> x) { return !p(v, x); });
}

template <typename I, typename V>
// requires ForwardIterator<I> && WeakComarable<ValueType<I>, V>
I upper_bound_biased_expensive_cmp(I f, I l, const V& v) {
  return upper_bound_biased_expensive_cmp(f, l, v, less{});
}

template <typename I, typename V, typename P>
// requires ForwardIterator<I> && StrictWeakOrder<P(ValueType<I>, V)>
std::pair<I, I> equal_range_biased_expensive_cmp(I f, I l, const V& v, P p) {
  auto lb = lower_bound_biased_expensive_cmp(f, l, v, p);
  auto ub = upper_bound_biased_expensive_cmp(lb, l, v, p);
  return {lb, ub};
}

template <typename I, typename V>
// requires ForwardIterator<I> && WeakComarable<ValueType<I>, V>
std::pair<I, I> equal_range_biased_expensive_cmp(I f, I l, const V& v) {
  return equal_range_biased_expensive_cmp(f, l, v, less{});
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

template <typename I, typename V, typename P>
// requires ForwardIterator<I> && StrictWeakOrder<P(ValueType<I>, V)>
I upper_bound_biased(I f, I l, const V& v, P p) {
  return partition_point_biased(f, l, [&](Reference<I> x) { return !p(v, x); });
}

template <typename I, typename V>
// requires ForwardIterator<I> && WeakComarable<ValueType<I>, V>
I upper_bound_biased(I f, I l, const V& v) {
  return upper_bound_biased(f, l, v, less{});
}

template <typename I, typename V, typename P>
// requires ForwardIterator<I> && StrictWeakOrder<P(ValueType<I>, V)>
std::pair<I, I> equal_range_biased(I f, I l, const V& v, P p) {
  auto lb = lower_bound_biased(f, l, v, p);
  auto ub = upper_bound_biased(lb, l, v, p);
  return {lb, ub};
}

template <typename I, typename V>
// requires ForwardIterator<I> && WeakComarable<ValueType<I>, V>
std::pair<I, I> equal_range_biased(I f, I l, const V& v) {
  return equal_range_biased(f, l, v, less{});
}

template <typename I>
struct range_pair : std::pair<I, I> {
  using base = std::pair<I, I>;

  using iterator = I;
  using reverse_iterator = std::reverse_iterator<iterator>;

  using base::base;
  iterator begin() const { return base::first; }
  iterator cbegin() const { return begin(); }

  iterator end() const { return base::second; }
  iterator cend() const { return end(); }

  reverse_iterator rbegin() const { return reverse_iterator(end()); }
  reverse_iterator crbegin() const { return rbegin(); }

  reverse_iterator rend() const { return reverse_iterator(begin()); }
  reverse_iterator crend() const { return rend(); }
};

template <typename I, typename P>
// requires ForwardIterator<I> && StrictWeakOrder<P(ValueType<I>)>
class group_equals_iterator : P {
  I f_;
  I l_;
  range_pair<I> cur_;

  void update_cur(I start) {
    cur_.first = start;
    if (start == l_) {
      cur_.second = l_;
      return;
    }
    cur_.second = upper_bound_biased(start, l_, *cur_.first, P(*this));
  }

  void update_cur(std::reverse_iterator<I> end) {
    cur_.second = end.base();
    std::reverse_iterator<I> reverse_f(f_);

    if (end == reverse_f) {
      cur_.first = f_;
      return;
    }

    auto reverse_p = [&](const auto& x, const auto& y) {
      return P(*this)(y, x);
    };

    cur_.first = upper_bound_biased(end, reverse_f, *end, reverse_p).base();
  }

 public:
  using difference_type = DifferenceType<I>;
  using value_type = range_pair<I>;
  using pointer = const value_type*;
  using reference = const value_type&;
  using iterator_category =
      std::conditional_t<is_bidirectional_v<I>, std::bidirectional_iterator_tag,
                         std::forward_iterator_tag>;

  group_equals_iterator(I f, I l, I pos, P p) : P(p), f_(f), l_(l) {
    update_cur(pos);
  }

  reference operator*() const { return cur_; }
  pointer operator->() const { return &cur_; }

  group_equals_iterator& operator++() {
    update_cur(cur_.second);
    return *this;
  }

  group_equals_iterator operator++(int) {
    auto tmp = *this;
    ++(*this);
    return tmp;
  }

  group_equals_iterator& operator--() {
    update_cur(std::reverse_iterator<I>(cur_.first));
    return *this;
  }

  group_equals_iterator operator--(int) {
    auto tmp = *this;
    --(*this);
    return tmp;
  }

  friend bool operator==(const group_equals_iterator& x,
                         const group_equals_iterator& y) {
    return x.cur_ == y.cur_;
  }

  friend bool operator!=(const group_equals_iterator& x,
                         const group_equals_iterator& y) {
    return !(x == y);
  }
};

template <typename I, typename P>
// requires ForwardIterator<I> && StrictWeakOrder<P(ValueType<I>)>
range_pair<group_equals_iterator<I, P>> group_equals(I f, I l, P p) {
  return {group_equals_iterator<I, P>{f, l, f, p},
          group_equals_iterator<I, P>{f, l, l, p}};
}

}  // namespace srt