#pragma once

#include "../stride/stride.hpp"
#include "../view/view.hpp"

#include <ranges>
#include <string_view>
#include <utility>

namespace xmas {
namespace views {

struct text_matrix {
  text_matrix(std::string& data);

  [[nodiscard]] view<std::string::iterator> row(std::size_t i);
  [[nodiscard]] std::string_view line(std::size_t i) const;

  [[nodiscard]] strided<std::string::iterator> col(std::size_t j);

  [[nodiscard]] char at(std::size_t i, std::size_t j) const;
  [[nodiscard]] char& at(std::size_t i, std::size_t j);

  [[nodiscard]] char at(auto pair) const {
    const auto& [row, col] = pair;
    return this->at(row, col);
  }

  [[nodiscard]] char& at(auto pair) {
    const auto& [row, col] = pair;
    return this->at(row, col);
  }

  [[nodiscard]] std::size_t nrows() const noexcept {
    return n_rows;
  }
  [[nodiscard]] std::size_t ncols() const noexcept {
    return n_cols;
  }
  [[nodiscard]] std::size_t size() const noexcept {
    return data().size();
  }

  std::string_view data() const {
    return text;
  }

  auto rows() {
    return std::ranges::views::iota(std::size_t{0}, n_rows) |
           std::ranges::views::transform([this](std::size_t r) { return row(r); });
  }

  auto lines() const {
    return std::ranges::views::iota(std::size_t{0}, n_rows) |
           std::ranges::views::transform([this](std::size_t r) { return line(r); });
  }

  auto cols() {
    return std::ranges::views::iota(std::size_t{0}, n_cols) |
           std::ranges::views::transform([this](std::size_t c) { return col(c); });
  }

private:
  std::size_t n_rows;
  std::size_t n_cols;
  std::string& text;

  static std::pair<std::size_t, std::size_t> dimensions(std::string_view input);
};

} // namespace views
} // namespace xmas