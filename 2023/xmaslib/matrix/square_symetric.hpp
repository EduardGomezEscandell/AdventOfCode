#pragma once

#include <cassert>
#include <vector>

namespace xmas {

template <typename T> class square_symetric_matrix {
public:
  square_symetric_matrix(std::size_t size)
      : m_data(translate(size, 0)), n(size) {}

  square_symetric_matrix(std::size_t size, T value)
      : m_data(translate(size, 0), value), n(size) {}

  T &at(std::size_t i, std::size_t j) {
    assert(i < n);
    assert(j < n);
    return m_data[translate(i, j)];
  }

  T at(std::size_t i, std::size_t j) const {
    assert(i < n);
    assert(j < n);
    return m_data[translate(i, j)];
  }

  auto &data() { return m_data; }

private:
  std::vector<T> m_data;
  std::size_t n;

  std::size_t translate(std::size_t row, std::size_t col) const {
    if (row < col) {
      std::swap(row, col);
    }
    return ((row + 1) * row) / 2 + col; // <3 Gauss
  }
};

} // namespace xmas