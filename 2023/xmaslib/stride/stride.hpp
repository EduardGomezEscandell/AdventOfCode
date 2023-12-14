#pragma once

#include <cassert>
#include <cstddef>
#include <format>
#include <iterator>
#include <stdexcept>

#ifndef NDEBUG
#define safe_cast(type, in) safe_cast_impl<type>(in, __PRETTY_FUNCTION__)
#else
#define safe_cast(type, in) static_cast<type>(in)
#endif

namespace xmas {
namespace views {

template <typename Base> class strided {
  // Ensure base iterator is random_access
  static_assert(
      std::is_same<typename std::iterator_traits<Base>::iterator_category,
                   std::random_access_iterator_tag>::value);

public:
  class iterator;

  template <std::integral S>
  strided(Base begin, Base end, S stride)
      : m_begin{begin, stride}, m_end{aligned(begin, end, stride), stride} {
    assert(begin < end);
  }

  [[nodiscard]] auto begin() const { return m_begin; }
  [[nodiscard]] auto end() const { return m_end; }
  [[nodiscard]] auto size() const {
    return safe_cast(std::size_t, m_end - m_begin);
  }

private:
  // Align an iterator to the next stride
  template <std::integral S>
  [[nodiscard]] static Base aligned(Base begin, Base it, S stride) {
    const auto delta = safe_cast(std::size_t, it - begin);
    return begin +
           safe_cast(std::ptrdiff_t, stride * div_round_up(delta, stride));
  }

  [[nodiscard]] static std::size_t div_round_up(std::size_t x, std::size_t y) {
    return x / y + (x % y != 0);
  }

  iterator m_begin;
  iterator m_end;

  template <typename Out, typename In>
  constexpr static Out safe_cast_impl(In in, std::string_view funcname) {
    Out out = static_cast<Out>(in);
    if (in != static_cast<In>(out)) {
      throw std::runtime_error(
          std::format("{}: Failed cast {} != {}", funcname, in, out));
    }
    return out;
  }

public:
  class iterator {
  public:
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::iterator_traits<Base>::difference_type;
    using value_type = std::iterator_traits<Base>::value_type;
    using pointer = std::iterator_traits<Base>::pointer;
    using reference = std::iterator_traits<Base>::reference;

    template <std::integral S>
    iterator(Base it, S stride) noexcept : base{it}, stride{stride} {
      assert(stride > 0);
    }

    iterator operator++() noexcept {
      base += stride;
      return *this;
    }

    iterator operator++(int) noexcept {
      auto it = iterator{base, stride};
      ++*this;
      return it;
    }

    iterator operator--() noexcept {
      base -= stride;
      return *this;
    }

    iterator operator--(int) noexcept {
      auto it = iterator{base, stride};
      --*this;
      return it;
    }

    iterator &operator+=(std::ptrdiff_t delta) noexcept {
      base += delta * stride;
      return *this;
    }

    iterator &operator-=(std::ptrdiff_t delta) noexcept {
      base -= delta * stride;
      return *this;
    }

    [[nodiscard]] iterator operator+(std::ptrdiff_t delta) const noexcept {
      auto it = *this;
      return it += delta;
    }

    [[nodiscard]] iterator operator-(std::ptrdiff_t delta) const noexcept {
      auto it = *this;
      return it -= delta;
    }

    [[nodiscard]] difference_type operator-(iterator other) const noexcept {
      const auto diff = this->base - other.base;
      const auto s = safe_cast(std::ptrdiff_t, stride);
      assert(diff % s == 0); // Mismatching iterators
      return diff / s;
    }

    [[nodiscard]] bool operator==(iterator other) const noexcept {
      return base == other.base;
    }

    [[nodiscard]] bool operator!=(iterator other) const noexcept {
      return !(*this == other);
    }

    [[nodiscard]] std::strong_ordering
    operator<=>(iterator other) const noexcept {
      return base <=> other.base;
    }

    [[nodiscard]] value_type operator*() const noexcept { return *base; }

    [[nodiscard]] value_type operator[](std::ptrdiff_t index) const noexcept {
      return *(base + index * safe_cast(difference_type, stride));
    }

  private:
    Base base;
    std::size_t stride;
  };
};
} // namespace views
} // namespace xmas

#undef safe_cast