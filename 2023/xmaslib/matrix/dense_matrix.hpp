#pragma once

#include "../stride/stride.hpp"
#include "../view/view.hpp"
#include "../lazy_string/lazy_string.hpp"

#include "dense_algebra.hpp"
#include "dense_vector.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <execution>
#include <format>
#include <ranges>
#include <sstream>
#include <type_traits>
#include <vector>

namespace xmas {

template <typename T>
class basic_matrix {
public:
  basic_matrix(std::size_t nrows, std::size_t ncols) :
      m_data(nrows * ncols), n_rows(nrows), n_cols(ncols) {
  }

  basic_matrix(std::size_t nrows, std::size_t ncols, T const& value) :
      m_data(nrows * ncols, value), n_rows(nrows), n_cols(ncols) {
  }

  template <typename Iterator>
  basic_matrix(Iterator begin, Iterator end, std::size_t nrows, std::size_t ncols) :
      m_data(begin, end), n_rows(nrows), n_cols(ncols) {
    assert(end - begin == n_rows * n_cols);
  }

  std::size_t size() const noexcept {
    return data().size();
  }

  std::size_t nrows() const noexcept {
    return n_rows;
  }

  std::size_t ncols() const noexcept {
    return n_cols;
  }

  auto row(std::size_t r) {
    assert(r < n_rows);
    return xmas::view{
      data().begin() + static_cast<std::ptrdiff_t>(r * n_cols),
      data().begin() + static_cast<std::ptrdiff_t>((r + 1) * n_cols),
    };
  }

  auto row(std::size_t r) const {
    assert(r < n_rows);
    return xmas::view{
      data().cbegin() + static_cast<std::ptrdiff_t>(r * n_cols),
      data().cbegin() + static_cast<std::ptrdiff_t>((r + 1) * n_cols),
    };
  }

  auto col(std::size_t c) const {
    assert(c < n_cols);
    return xmas::views::strided{
      data().cbegin() + static_cast<std::ptrdiff_t>(c),
      data().cend(),
      ncols(),
    };
  }

  auto crow(std::size_t r) const {
    return row(r);
  }

  auto ccol(std::size_t r) const {
    return col(r);
  }

  auto operator[](std::size_t r) {
    return row(r);
  }

  auto operator[](std::size_t r) const {
    return row(r);
  }

  template <typename TupleLike>
  std::enable_if<!std::is_integral_v<TupleLike>, T>::type operator[](TupleLike rc) const {
    auto const& [r, c] = rc;
    return row(r)[c];
  }

  template <typename TupleLike>
  std::enable_if<!std::is_integral_v<TupleLike>, T&>::type operator[](TupleLike rc) {
    auto const& [r, c] = rc;
    return row(r)[c];
  }

  auto& data() noexcept {
    return m_data;
  }

  auto const& data() const noexcept {
    return cdata();
  }

  auto const& cdata() const noexcept {
    return m_data;
  }

  auto begin() {
    return data().begin();
  }
  auto begin() const {
    return cbegin();
  }
  auto cbegin() const {
    return data().cbegin();
  }
  auto rbegin() {
    return data().rbegin();
  }
  auto rbegin() const {
    return rcbegin();
  }
  auto rcbegin() const {
    return data().rcbegin();
  }

  auto end() {
    return data().end();
  }
  auto end() const {
    return cend();
  }
  auto cend() const {
    return data().cend();
  }
  auto rend() {
    return data().rend();
  }
  auto rend() const {
    return rcend();
  }
  auto rcend() const {
    return data().rcend();
  }

  auto rows() noexcept {
    return std::ranges::views::iota(std::size_t(0), n_rows) |
           std::ranges::views::transform([this](std::size_t r) { return row(r); });
  }

  auto rows() const noexcept {
    return crows();
  }

  auto crows() const noexcept {
    return std::ranges::views::iota(std::size_t(0), n_rows) |
           std::ranges::views::transform([this](std::size_t r) { return crow(r); });
  }

  auto cols() noexcept {
    return std::ranges::views::iota(std::size_t(0), n_cols) |
           std::ranges::views::transform([this](std::size_t r) { return col(r); });
  }

  auto cols() const noexcept {
    return ccols();
  }

  auto ccols() const noexcept {
    return std::ranges::views::iota(std::size_t(0), n_cols) |
           std::ranges::views::transform([this](std::size_t r) { return ccol(r); });
  }

  basic_matrix& transpose() {
    std::vector<T> d(data().size());
    xmas::view v(d.begin(), d.end());
    for (auto c : cols()) {
      elementwise(c, v, [](T t) { return t; });
      v.pop_front(nrows());
    }
    m_data = std::move(d);
    std::swap(n_rows, n_cols);
    return *this;
  }

  [[nodiscard]] basic_matrix<T> transposed() const {
    auto out = basic_matrix<T>(ncols(), nrows());
    xmas::view v(out.begin(), out.end());
    for (auto c : cols()) {
      elementwise(c, v, [](T t) { return t; });
      v.pop_front(nrows());
    }
    return out;
  }

  basic_matrix& operator+=(basic_matrix const& other) {
    elementwise(m_data, other.m_data, m_data, [](T t, T o) { return t + o; });
    return *this;
  }

  basic_matrix& operator-=(basic_matrix const& other) {
    elementwise(m_data, other.m_data, m_data, [](T t, T o) { return t - o; });
    return *this;
  }

  basic_matrix operator+(basic_matrix const& other) {
    basic_matrix out(n_rows, n_cols);
    elementwise(m_data, other.m_data, out, [](T t, T o) { return t + o; });
    return out;
  }

  basic_matrix operator-(basic_matrix const& other) {
    basic_matrix out(n_rows, n_cols);
    elementwise(m_data, other.m_data, out, [](T t, T o) { return t - o; });
    return out;
  }

  basic_matrix operator*(basic_matrix const& other) const {
    assert(n_cols == other.n_rows);
    basic_matrix out(n_rows, other.n_cols);
    for (std::size_t r = 0; r < out.n_rows; ++r) {
      for (std::size_t c = 0; c < out.n_cols; ++c) {
        out[r][c] = algebra::inner(row(r), other.col(c));
      }
    }
    return out;
  }

  basic_matrix& operator*=(T a) {
    elementwise(data(), data(), [a](T t) { return a * t; });
    return *this;
  }

  basic_matrix<T> operator*(T a) const {
    auto out = basic_matrix<T>(nrows(), ncols());
    elementwise(cdata(), out.data(), [a](T t) { return a * t; });
    return out;
  }

  basic_matrix& operator/=(T a) {
    elementwise(data(), data(), [a](T t) { return a / t; });
    return *this;
  }

  basic_matrix<T> operator/(T k) const {
    auto out = basic_matrix<T>(size());
    elementwise(cdata(), out.data(), [k](T t) { return k / t; });
    return out;
  }

  static basic_matrix identity(std::size_t n) {
    basic_matrix out(n, n);
    for (std::size_t i = 0; i < n; ++i) {
      out[i][i] = T{1};
    }
    return out;
  }

  template <typename TupleLike1 = std::pair<std::size_t, std::size_t>,
    typename TupleLike2 = std::pair<std::size_t, std::size_t>>
  basic_matrix<T> submatrix(TupleLike1 const& row_range, TupleLike2 const& col_range) {
    auto const [r0, r1] = row_range;
    auto const [c0, c1] = col_range;

    assert(r1 > r0);
    assert(c1 > c0);

    assert(r1 <= n_rows);
    assert(c1 <= n_cols);

    basic_matrix<T> B(r1 - r0, c1 - c0);
    for (std::size_t r = r0; r < r1; ++r) {
      for (std::size_t c = c0; c < c1; ++c) {
        B[r - r0][c - c0] = (*this)[r][c];
      }
    }

    return B;
  }

  auto format(std::string_view fmt = "{:>4}") const {
    return lazy_string([this, fmt] {
      std::stringstream ss;
      for (auto r : crows()) {
        ss << '[';
        for (T t : r) {
          ss << ' ' << std::vformat(fmt, std::make_format_args(t));
        }
        ss << " ]\n";
      }
      return std::move(ss).str();
    });
  }

private:
  std::vector<T> m_data;
  std::size_t n_rows, n_cols;

  static void elementwise(auto const& in_left, auto const& in_right, auto& out, auto&& op) {
    assert(in_left.size() <= in_right.size());
    assert(in_left.size() <= out.size());
    std::transform(
      std::execution::unseq, in_left.begin(), in_left.end(), in_right.begin(), out.begin(), op);
  }

  static void elementwise(auto const& in, auto& out, auto&& op) {
    assert(in.size() <= out.size());
    std::transform(std::execution::unseq, in.begin(), in.end(), out.begin(), op);
  }
};

using matrix = basic_matrix<float>;

template <typename T>
basic_matrix<T> operator*(T k, basic_matrix<T> const& A) {
  return A * k;
}

// Matrix-vector product
template <typename T>
basic_vector<T> operator*(basic_matrix<T> const& A, auto const& u) {
  assert(A.ncols() == u.size());
  basic_vector<T> out(A.nrows());
  std::transform(A.crows().begin(), A.crows().end(), out.begin(),
    [&u](auto const& row) { return algebra::inner(row, u); });
  return out;
}

// Vector-matrix product
template <typename T>
basic_vector<T> operator*(auto const& u, basic_matrix<T> const& A) {
  assert(A.nrows() == u.size());
  basic_vector<T> out(A.ncols());
  std::transform(A.cols().begin(), A.cols().end(), out.begin(),
    [&u](auto const& col) { return algebra::inner(u, col); });
  return out;
}

template <typename T>
basic_vector<T> operator*(basic_matrix<T> const& A, basic_vector<T> const& u) {
  return A * u.cdata();
}

template <typename T>
basic_vector<T> operator*(basic_vector<T> const& u, basic_matrix<T> const& A) {
  return u.data() * A;
}

}
