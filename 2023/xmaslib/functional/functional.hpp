#pragma once

#include <algorithm>
namespace xmas {

template <typename T> class equals {
public:
  explicit constexpr equals(T value) : value(value) {}

  constexpr bool operator()(T const &other) const { return value == other; }

private:
  T value;
};

template <typename T> class less_than {
public:
  explicit constexpr less_than(T value) : value(value) {}

  constexpr bool operator()(T const &other) const { return other < value; }

private:
  T value;
};

template <typename T> class min {
public:
  constexpr bool operator()(T const &l, T const &r) const {
    return std::min(l, r);
  }
};

template <typename T> class max {
public:
  constexpr T operator()(T const &l, T const &r) const {
    return std::max(l, r);
  }
};

} // namespace xmas