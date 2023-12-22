#pragma once

#include "../stride/stride.hpp"

#include <ranges>
#include <vector>

namespace xmas {

template <typename T>
struct matrix {
  matrix(std::size_t nrows, std::size_t ncols) :
      m_data(nrows * ncols), n_cols(ncols), n_rows(ncols) {
  }

  matrix(std::size_t nrows, std::size_t ncols, T const& value) :
      m_data(nrows * ncols, value), n_rows(nrows), n_cols(ncols) {
  }

  std::size_t size() const noexcept {
    return m_data.size();
  }

  std::size_t nrows() const noexcept {
    return n_rows;
  }

  std::size_t ncols() const noexcept {
    return n_cols;
  }

  std::span<T> row(std::size_t r) {
    assert(r < n_rows);
    return {
      m_data.begin() + static_cast<std::ptrdiff_t>(r * n_cols),
      m_data.begin() + static_cast<std::ptrdiff_t>((r + 1) * n_cols),
    };
  }

  std::span<const T> row(std::size_t r) const {
    assert(r < n_rows);
    return {
      m_data.begin() + static_cast<std::ptrdiff_t>(r * n_cols),
      m_data.begin() + static_cast<std::ptrdiff_t>((r + 1) * n_cols),
    };
  }

  auto col(std::size_t c) {
    assert(c < n_cols);
    return xmas::views::strided{
      m_data.begin() + static_cast<std::ptrdiff_t>(c),
      m_data.end(),
    };
  }

  auto col(std::size_t c) const {
    assert(c < n_cols);
    return xmas::views::strided{
      m_data.cbegin() + static_cast<std::ptrdiff_t>(c),
      m_data.cend(),
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

  std::span<T> data() noexcept {
    return m_data;
  }

  std::span<const T> data() const noexcept {
    return m_data;
  }

  std::span<const T> cdata() const noexcept {
    return m_data;
  }

  auto rows() noexcept {
    return std::ranges::views::iota(std::size_t(0), n_rows) |
           std::ranges::views::transform([this](std::size_t r) { return row(r); });
  }

  auto crows() const noexcept {
    return std::ranges::views::iota(std::size_t(0), n_rows) |
           std::ranges::views::transform([this](std::size_t r) { return crow(r); });
  }

  auto cols() noexcept {
    return std::ranges::views::iota(std::size_t(0), n_cols) |
           std::ranges::views::transform([this](std::size_t r) { return col(r); });
  }

  auto ccols() const noexcept {
    return std::ranges::views::iota(std::size_t(0), n_cols) |
           std::ranges::views::transform([this](std::size_t r) { return ccol(r); });
  }

private:
  std::vector<T> m_data;
  std::size_t n_rows, n_cols;
};

}