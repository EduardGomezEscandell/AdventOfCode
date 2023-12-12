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

  constexpr iota(T begin, T end) : m_begin(begin), m_end(end) {
    assert(begin <= end);
  }
  constexpr iota(T end) : iota(0, end) {}
  constexpr iota() : iota(0, std::numeric_limits<T>::max()) {}

  constexpr iterator begin() const noexcept { return cbegin(); }
  constexpr iterator end() const noexcept { return cend(); }

  constexpr iterator cbegin() const noexcept { return iterator(m_begin); }
  constexpr iterator cend() const noexcept { return iterator(m_end); }

  constexpr std::size_t size() const noexcept {
    return safe_cast<std::size_t>(m_end - m_begin);
  }

  constexpr T front() const noexcept { return m_begin; }
  constexpr T back() const noexcept { return m_end - 1; }

  constexpr T operator[](std::size_t index) const noexcept {
#ifndef NDEBUG
    range_check(index);
#endif
    return safe_cast<T>(m_begin + safe_cast<std::ptrdiff_t>(index));
  }

  constexpr T at(std::size_t index) const {
    range_check(index);
    return safe_cast<T>(m_begin + safe_cast<std::ptrdiff_t>(index));
  }

private:
  constexpr void range_check(std::size_t index) const {
    if (index >= size()) {
      throw std::runtime_error(
          std::format("xmas::views::iota::range_check: index (which is %zu) "
                      ">= this->size()  (which is %zu)",
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
      auto it = iterator{v + 1};
      ++*this;
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
    operator[](std::size_t index) const noexcept {
      return safe_cast<value_type>(v + safe_cast<std::ptrdiff_t>(index));
    }

  private:
    T v;
  };
};

} // namespace views
} // namespace xmas