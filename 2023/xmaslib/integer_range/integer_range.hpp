#pragma once

#include <algorithm>
#include <cassert>
#include <iterator>
#include <utility>
#include <vector>

namespace xmas {

template <std::integral T>
struct integer_range {
public:
  T begin, end;

  constexpr integer_range() = default;

  constexpr integer_range(T end) : integer_range(0, end) {
  }

  constexpr integer_range(T begin, T end) : begin(begin), end(end) {
    assert(begin <= end);
  }

  constexpr integer_range(auto pair) {
    const auto& [begin, end] = pair;
    integer_range(begin, end);
  }

  constexpr bool empty() const noexcept {
    return begin == end;
  }

  constexpr std::size_t size() const noexcept {
    return static_cast<std::size_t>(end - begin);
  }

  constexpr bool contains(T query) const {
    return (query >= begin) && (query < end);
  }

  // split returns ranges [begin, splitpoint) and [splitpoint, end).
  // If splitpoint is not inside [begin, end), the behaviour is undefined.
  constexpr std::pair<integer_range, integer_range> split(T splitpoint) const {
    return {
      integer_range(begin, splitpoint),
      integer_range(splitpoint, end),
    };
  }

  // Modifies *this to become [splitpoint, end) and returns [begin, splitpoint).
  // If splitpoint is not inside [begin, end), the behaviour is undefined.
  constexpr integer_range drop_less(T splitpoint) {
    auto r = integer_range{begin, splitpoint};
    this->begin = splitpoint;
    return r;
  }

  // Modifies *this to become [splitpoint+1, end) and returns [begin, splitpoint+1).
  // If splitpoint is not inside [begin, end), the behaviour is undefined.
  constexpr integer_range drop_less_equal(T splitpoint) {
    auto r = integer_range{begin, splitpoint + 1};
    this->begin = splitpoint + 1;
    return r;
  }

  // Modifies *this to become [begin, splitpoint) and returns [splitpoint, end).
  // If splitpoint is not inside [begin, end), the behaviour is undefined.
  constexpr integer_range keep_less(T splitpoint) {
    auto r = integer_range{splitpoint, end};
    this->end = splitpoint;
    return r;
  }

  // Modifies *this to become [begin, splitpoint+1) and returns [splitpoint+1, end).
  // If splitpoint is not inside [begin, end), the behaviour is undefined.
  constexpr integer_range keep_less_equal(T splitpoint) {
    auto r = integer_range{splitpoint + 1, end};
    this->end = splitpoint + 1;
    return r;
  }

  // Modifies *this to become [splitpoint, end) and returns [begin, splitpoint).
  // If splitpoint is not inside [begin, end), the behaviour is undefined.
  constexpr integer_range keep_greater_equal(T splitpoint) {
    return drop_less(splitpoint);
  }

  // Modifies *this to become [splitpoint+1, end) and returns [begin, splitpoint+1).
  // If splitpoint is not inside [begin, end), the behaviour is undefined.
  constexpr integer_range keep_greater(T splitpoint) {
    return drop_less_equal(splitpoint);
  }

  // Modifies *this to become [begin, splitpoint) and returns [splitpoint, end).
  // If splitpoint is not inside [begin, end), the behaviour is undefined.
  constexpr integer_range drop_greater_equal(T splitpoint) {
    return keep_less(splitpoint);
  }

  // Modifies *this to become [begin, splitpoint+1) and returns [splitpoint+1, end).
  // If splitpoint is not inside [begin, end), the behaviour is undefined.
  constexpr integer_range drop_greater(T splitpoint) {
    return keep_less_equal(splitpoint);
  }

  constexpr integer_range overlap(integer_range other) const {
    auto begin = std::max(this->begin, other.begin);
    auto end = std::min(this->end, other.end);
    if (begin < end) {
      return {begin, end};
    }
    return {0, 0};
  }

private:
  static constexpr integer_range build_or_make_empty(T begin, T end) {
    if (begin < end) {
      return {begin, end};
    }
    return {end, end};
  }
};

// coalesce_ranges takes a view of integer ranges and merges overlapping ones in-place.
// Returns an iterator past the end of the coalesced ranges.
template <typename Iterator,
  std::integral T = decltype(typename std::iterator_traits<Iterator>::value_type{}.begin)>
Iterator coalesce_ranges(Iterator begin, Iterator end) {
  if (end - begin < 2) {
    return end;
  }

  // Sort according to range start
  std::sort(begin, end,
    [](integer_range<T> const& a, integer_range<T> const& b) { return a.begin < b.begin; });

  // Merge any 2 or more consecutive ranges where prev.end >= next.begin
  auto ok_end = std::next(begin);
  for (auto it = ok_end; it != end; ++it) {
    integer_range<T>& curr = *it;
    integer_range<T>& prev = *std::prev(it);
    if (prev.end >= curr.begin) {
      // Overlap with previous range: extend the previous one to absorb this one
      prev.end = std::max(prev.end, curr.end);
      continue;
    }
    // No overlap: append
    std::swap(*ok_end, *it);
    ++ok_end;
  }

  return ok_end;
}

// coalesce_ranges takes a view of integer ranges and merges overlapping ones in-place.
// This is a convenience implementation for vectors.
template <typename T>
void coalesce_ranges(std::vector<integer_range<T>>& v) {
  auto end = xmas::coalesce_ranges(v.begin(), v.end());
  v.erase(end, v.end());
}

}