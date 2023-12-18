#pragma once

#include <algorithm>
#include <utility>
namespace xmas {

template <typename T>
class identity {
public:
  constexpr T const& operator()(T const& t) const {
    return t;
  }

  constexpr T&& operator()(T&& t) const {
    return std::forward<T>(t);
  }

  constexpr T operator()(T t) const {
    return t;
  }
};

template <typename T>
class equals {
public:
  explicit constexpr equals(T value) : value(value) {
  }

  constexpr bool operator()(T const& other) const {
    return value == other;
  }

private:
  T value;
};

template <typename T>
class less_than {
public:
  explicit constexpr less_than(T value) : value(value) {
  }

  constexpr bool operator()(T const& other) const {
    return other < value;
  }

private:
  T value;
};

template <typename T>
class greater_than {
public:
  explicit constexpr greater_than(T value) : value(value) {
  }

  constexpr bool operator()(T const& other) const {
    return other > value;
  }

private:
  T value;
};

template <typename T>
class less_or_equal_than {
public:
  explicit constexpr less_or_equal_than(T value) : value(value) {
  }

  constexpr bool operator()(T const& other) const {
    return other <= value;
  }

private:
  T value;
};

template <typename T>
class min {
public:
  constexpr bool operator()(T const& l, T const& r) const {
    return std::min(l, r);
  }
};

template <typename T>
class max {
public:
  constexpr T operator()(T const& l, T const& r) const {
    return std::max(l, r);
  }
};

} // namespace xmas