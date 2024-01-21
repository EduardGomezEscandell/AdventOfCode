#pragma once

#include "../stride/stride.hpp"
#include "../view/view.hpp"
#include "../lazy_string/lazy_string.hpp"

#include "dense_vector.hpp"
#include "xmaslib/iota/iota.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <execution>
#include <format>
#include <functional>
#include <numeric>
#include <ranges>
#include <sstream>
#include <type_traits>
#include <vector>

namespace xmas {

template <typename T>
class base_matrix { };

template <typename M, typename T>
concept is_matrix = std::derived_from<M, base_matrix<T>>;

template <typename T>
class transposed_view;

template <typename T>
class dense_matrix : public base_matrix<T> {
public:
  dense_matrix(std::size_t nrows, std::size_t ncols) :
      m_data(nrows * ncols), n_rows(nrows), n_cols(ncols) {
  }

  dense_matrix(std::size_t nrows, std::size_t ncols, T const& value) :
      m_data(nrows * ncols, value), n_rows(nrows), n_cols(ncols) {
  }

  template <typename Iterator>
  dense_matrix(Iterator begin, Iterator end, std::size_t nrows, std::size_t ncols) :
      m_data(begin, end), n_rows(nrows), n_cols(ncols) {
    assert(end - begin == n_rows * n_cols);
  }

  dense_matrix(transposed_view<T> view) : dense_matrix(view.nrows(), view.ncols()) {
    for (std::size_t i = 0; i < n_rows; ++i) {
      for (std::size_t j = 0; j < n_rows; ++j) {
        *this[i][j] = view[i][j];
      }
    }
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

  dense_matrix& transpose() {
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

  [[nodiscard]] transposed_view<T> transposed() const {
    return transposed_view<T>{*this};
  }

  dense_matrix& operator+=(dense_matrix const& other) {
    elementwise(m_data, other.m_data, m_data, [](T t, T o) { return t + o; });
    return *this;
  }

  dense_matrix& operator-=(dense_matrix const& other) {
    elementwise(m_data, other.m_data, m_data, [](T t, T o) { return t - o; });
    return *this;
  }

  dense_matrix operator+(dense_matrix const& other) {
    dense_matrix out(n_rows, n_cols);
    elementwise(m_data, other.m_data, out, [](T t, T o) { return t + o; });
    return out;
  }

  dense_matrix operator-(dense_matrix const& other) {
    dense_matrix out(n_rows, n_cols);
    elementwise(m_data, other.m_data, out, [](T t, T o) { return t - o; });
    return out;
  }

  dense_matrix operator*(is_matrix<T> auto const& other) const {
    assert(n_cols == other.n_rows);
    dense_matrix out(n_rows, other.n_cols);
    xmas::views::iota<std::size_t> iota(out.nrows());
    std::for_each(std::execution::par_unseq, iota.begin(), iota.end(), [&](std::size_t r) {
      for (std::size_t c = 0; c < out.ncols(); ++c) {
        out[r][c] = std::transform_reduce(std::execution::unseq, row(r).begin(), row(r).end(),
          other.col(c).begin(), T{0}, std::plus<T>{}, std::multiplies<T>{});
      }
    });
    return out;
  }

  dense_matrix& operator*=(T a) {
    elementwise(data(), data(), [a](T t) { return a * t; });
    return *this;
  }

  dense_matrix<T> operator*(T a) const {
    auto out = dense_matrix<T>(nrows(), ncols());
    elementwise(cdata(), out.data(), [a](T t) { return a * t; });
    return out;
  }

  dense_matrix& operator/=(T k) {
    elementwise(data(), data(), [k](T t) { return t / k; });
    return *this;
  }

  dense_matrix operator/(T k) const {
    auto out = dense_matrix(nrows(), ncols());
    elementwise(cdata(), out.data(), [k](T t) { return t / k; });
    return out;
  }

  static dense_matrix identity(std::size_t n) {
    dense_matrix out(n, n, T{0});
    for (std::size_t i = 0; i < n; ++i) {
      out[i][i] = T{1};
    }
    return out;
  }

  template <typename TupleLike1 = std::pair<std::size_t, std::size_t>,
    typename TupleLike2 = std::pair<std::size_t, std::size_t>>
  dense_matrix submatrix(TupleLike1 const& row_range, TupleLike2 const& col_range) {
    auto const [r0, r1] = row_range;
    auto const [c0, c1] = col_range;

    assert(r1 > r0);
    assert(c1 > c0);

    assert(r1 <= n_rows);
    assert(c1 <= n_cols);

    dense_matrix B(r1 - r0, c1 - c0);
    for (std::size_t r = r0; r < r1; ++r) {
      for (std::size_t c = c0; c < c1; ++c) {
        B[r - r0][c - c0] = (*this)[r][c];
      }
    }

    return B;
  }

  auto format(std::string_view fmt = "{}") const {
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

  static T inner_product(auto const& in_left, auto const& in_right) {
    return std::transform_reduce(std::execution::unseq, in_left.begin(), in_left.end(),
      in_right.begin(), T{0}, std::plus<T>{}, std::multiplies<T>{});
  }
};

template <typename T, typename Matrix>
dense_matrix<T> operator*(T k, Matrix const& A)
  requires is_matrix<Matrix, T>
{
  return A * k;
}

template <typename T>
class transposed_view : public base_matrix<T> {
  dense_matrix<T> base;

public:
  explicit transposed_view(dense_matrix<T> const& base) : base(base) {
  }

  std::size_t size() const noexcept {
    return base.size();
  }

  std::size_t nrows() const noexcept {
    return base.ncols();
  }

  std::size_t ncols() const noexcept {
    return base.nrows();
  }

  auto row(std::size_t r) {
    return base.col(r);
  }

  auto row(std::size_t r) const {
    return base.col(r);
  }

  auto col(std::size_t c) const {
    return base.row(c);
  }

  auto crow(std::size_t r) const {
    return base.ccol(r);
  }

  auto ccol(std::size_t r) const {
    return base.crow(r);
  }

  auto operator[](std::size_t r) {
    return base.col(r);
  }

  auto operator[](std::size_t r) const {
    return base.col(r);
  }

  template <typename TupleLike>
  std::enable_if<!std::is_integral_v<TupleLike>, T>::type operator[](TupleLike rc) const {
    auto const& [r, c] = rc;
    return row(c)[r];
  }

  template <typename TupleLike>
  std::enable_if<!std::is_integral_v<TupleLike>, T&>::type operator[](TupleLike rc) {
    auto const& [r, c] = rc;
    return row(c)[r];
  }

  auto begin() {
    return base.begin();
  }
  auto begin() const {
    return base.begin();
  }
  auto cbegin() const {
    return base.begin();
  }

  auto end() {
    return base.end();
  }
  auto end() const {
    return cend();
  }
  auto cend() const {
    return base.cend();
  }

  auto rows() noexcept {
    return base.cols();
  }

  auto rows() const noexcept {
    return base.cols();
  }

  auto crows() const noexcept {
    return base.ccols();
  }

  auto cols() noexcept {
    return base.rows();
  }

  auto cols() const noexcept {
    return base.rows();
  }

  auto ccols() const noexcept {
    return base.crows();
  }

  [[nodiscard]] dense_matrix<T> transposed() const {
    return dense_matrix<T>{base};
  }

  dense_matrix<T> operator+(is_matrix<T> auto const& other) {
    dense_matrix<T> out(*this);
    return out += other;
  }

  dense_matrix<T> operator-(is_matrix<T> auto const& other) {
    dense_matrix<T> out(*this);
    return out -= other;
  }

  dense_matrix<T> operator*(is_matrix<T> auto const& other) const {
    assert(ncols() == other.nrows());
    dense_matrix<T> out(nrows(), other.ncols());

    xmas::views::iota<std::size_t> iota(out.nrows());
    std::for_each(std::execution::par_unseq, iota.begin(), iota.end(), [&](std::size_t r) {
      for (std::size_t c = 0; c < out.ncols(); ++c) {
        out[r][c] = std::transform_reduce(std::execution::unseq, row(r).begin(), row(r).end(),
          other.col(c).begin(), T{0}, std::plus<T>{}, std::multiplies<T>{});
      }
    });
    return out;
  }

  dense_matrix<T> operator*(T a) const {
    dense_matrix<T> out(*this);
    return out *= a;
  }

  dense_matrix<T> operator/(T k) const {
    dense_matrix<T> out(*this);
    return out /= k;
  }

  auto format(std::string_view fmt = "{}") const {
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
};
}
