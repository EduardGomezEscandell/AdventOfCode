#pragma once
#include <format>
#include <iterator>
#include <stdexcept>

#include <execinfo.h>

namespace xmas {

template <typename It> class view {
  It m_begin, m_end;

public:
  view(It begin, It end) : m_begin(begin), m_end(end) {}

  auto begin() const { return m_begin; }
  auto end() const { return m_end; }

  auto &front() const { return *m_begin; }
  auto size() const { return static_cast<std::size_t>(m_end - m_begin); }

  template <std::integral N = std::ptrdiff_t> void pop_front(N n = 1) {
    check_bounds(n, __PRETTY_FUNCTION__);
    m_begin += static_cast<std::ptrdiff_t>(n);
  }

  template <std::integral N = std::ptrdiff_t> void pop_back(N n = 1) {
    check_bounds(n, __PRETTY_FUNCTION__);
    m_end -= static_cast<std::ptrdiff_t>(n);
  }

  template <std::integral N = std::ptrdiff_t>
  [[nodiscard]] view subview(N begin, N end) const {
    return view{m_begin + static_cast<std::ptrdiff_t>(begin),
                m_begin + static_cast<std::ptrdiff_t>(end)};
  }

  template <std::integral N = std::ptrdiff_t>
  [[nodiscard]] view take(N n) const {
    check_bounds(n, __PRETTY_FUNCTION__);
    return view{m_begin, m_begin + static_cast<std::ptrdiff_t>(n)};
  }

  template <std::integral N = std::ptrdiff_t>
  [[nodiscard]] view drop(N n) const {
    check_bounds(n, __PRETTY_FUNCTION__);
    return view{m_begin + static_cast<std::ptrdiff_t>(n), m_end};
  }

  template <std::integral N = std::ptrdiff_t>
  [[nodiscard]] auto &operator[](N n) const {
    check_bounds(n, __PRETTY_FUNCTION__);
    return *(m_begin + static_cast<std::ptrdiff_t>(n));
  }

  [[nodiscard]] auto reverse() const {
    return view<std::reverse_iterator<It>>{std::reverse_iterator(m_end),
                                           std::reverse_iterator(m_begin)};
  }

private:
  void check_bounds([[maybe_unused]] std::integral auto n,
                    [[maybe_unused]] std::string_view funcname) const {
#ifndef NDEBUG
    if (n < 0) {
      throw std::runtime_error(
          std::format("{}: index (which is {}) < 0", funcname, n));
    }
    if (static_cast<std::size_t>(n) > size()) {
      throw std::runtime_error(
          std::format("{}: index (which is {}) > this->size() (which is {})",
                      funcname, n, this->size()));
    }
#endif
  }
};
} // namespace xmas