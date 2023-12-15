#pragma once

#include <cassert>
#include <compare>
#include <cstddef>
#include <format>
#include <iterator>
#include <limits>
#include <stdexcept>

namespace xmas {
namespace views {

// iota is a view that ranges over integers in [begin, end).
template <std::integral T> class iota {
public:
  class iterator;

  using size_type = std::size_t;

  constexpr iota(T begin, T end) : m_begin(begin), m_end(end) {
#ifndef NDEBUG
    if (begin > end) {
      throw std::runtime_error(std::format(
          "xmas::views::iota::iota begin (which is {}) >= end (which is {})",
          begin, end));
    }

#endif
  }

  constexpr iota(T end) : iota(0, end) {}
  constexpr iota() : iota(0, std::numeric_limits<T>::max()) {}

  constexpr iterator begin() const noexcept { return cbegin(); }
  constexpr iterator end() const noexcept { return cend(); }

  constexpr iterator cbegin() const noexcept { return iterator(m_begin); }
  constexpr iterator cend() const noexcept { return iterator(m_end); }

  constexpr size_type size() const noexcept {
    return safe_cast<size_type>(m_end - m_begin);
  }

  constexpr T front() const noexcept { return m_begin; }
  constexpr T back() const noexcept { return m_end - 1; }

  constexpr T operator[](std::integral auto index) const noexcept {
#ifndef NDEBUG
    range_check(index);
#endif
    return safe_cast<T>(safe_cast<std::ptrdiff_t>(m_begin) +
                        safe_cast<std::ptrdiff_t>(index));
  }

  constexpr T at(size_type index) const {
    range_check<false>(index);
    return safe_cast<T>(m_begin + index);
  }

private:
  template <bool debug_only = true>
  constexpr void range_check([[maybe_unused]] size_type index) const {
#ifndef NDEBUG
    constexpr bool is_debug = true;
#else
    constexpr bool is_debug = false;
#endif
    if constexpr (debug_only && !is_debug) {
      return;
    }

    if (static_cast<std::ptrdiff_t>(index) < 0) {
      throw std::runtime_error(
          std::format("xmas::views::iota::range_check: index (which is {}) "
                      "< 0",
                      index));
    }

    if (index >= size()) {
      throw std::runtime_error(
          std::format("xmas::views::iota::range_check: index (which is {}) "
                      ">= this->size()  (which is {})",
                      index, this->size()));
    }
  }

  T m_begin;
  T m_end;

  template <std::integral Out, std::integral In>
  constexpr static Out safe_cast(In in) {
    assert(in == static_cast<In>(static_cast<Out>(in)));
    return static_cast<Out>(in);
  }

public:
  class iterator {
  public:
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = void;
    using reference = value_type &;

    constexpr explicit iterator(value_type init = 0) noexcept : v{init} {}

    constexpr iterator operator++() noexcept {
      ++v;
      return *this;
    }

    constexpr iterator operator++(int) noexcept {
      auto it = iterator{v};
      ++*this;
      return it;
    }

    constexpr iterator operator--() noexcept {
      --v;
      return *this;
    }

    constexpr iterator operator--(int) noexcept {
      auto it = iterator{v};
      --*this;
      return it;
    }

    constexpr iterator &operator+=(std::ptrdiff_t delta) noexcept {
      v += safe_cast<value_type>(delta);
      return *this;
    }

    constexpr iterator &operator-=(std::ptrdiff_t delta) noexcept {
      v -= safe_cast<value_type>(delta);
      return *this;
    }

    [[nodiscard]] constexpr iterator
    operator+(std::ptrdiff_t delta) const noexcept {
      auto it = *this;
      return it += delta;
    }

    [[nodiscard]] constexpr iterator
    operator-(std::ptrdiff_t delta) const noexcept {
      auto it = *this;
      return it -= delta;
    }

    [[nodiscard]] constexpr difference_type
    operator-(iterator other) const noexcept {
      return safe_cast<difference_type>(this->v) -
             safe_cast<difference_type>(other.v);
    }

    [[nodiscard]] constexpr bool operator==(iterator other) const noexcept {
      return v == other.v;
    }

    [[nodiscard]] constexpr bool operator!=(iterator other) const noexcept {
      return !(*this == other);
    }

    [[nodiscard]] constexpr std::strong_ordering
    operator<=>(iterator other) const noexcept {
      return v <=> other.v;
    }

    [[nodiscard]] constexpr value_type operator*() const noexcept { return v; }

    [[nodiscard]] constexpr value_type
    operator[](size_type index) const noexcept {
      return safe_cast<value_type>(v + index);
    }

  private:
    T v;
  };
};

} // namespace views
} // namespace xmas