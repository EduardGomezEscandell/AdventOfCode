#pragma once

#include "../stride/stride.hpp"

#include <string_view>
#include <utility>

namespace xmas {
namespace views {

struct text_matrix {
  text_matrix(std::string_view data);

  [[nodiscard]] std::string_view row(std::size_t i) const;

  [[nodiscard]] strided<std::string_view::iterator> col(std::size_t j) const;

  [[nodiscard]] char at(std::size_t i, std::size_t j) const;

  [[nodiscard]] std::size_t nrows() const noexcept { return n_rows; }
  [[nodiscard]] std::size_t ncols() const noexcept { return n_cols; }

private:
  std::size_t n_rows;
  std::size_t n_cols;
  std::string_view text;

  static std::pair<std::size_t, std::size_t> dimensions(std::string_view input);
};

} // namespace views
} // namespace xmas