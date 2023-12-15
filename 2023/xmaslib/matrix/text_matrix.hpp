#pragma once

#include "../stride/stride.hpp"
#include "../view/view.hpp"

#include <string_view>
#include <utility>

namespace xmas {
namespace views {

struct text_matrix {
  text_matrix(std::string &data);

  [[nodiscard]] view<std::string::iterator> row(std::size_t i);
  [[nodiscard]] strided<std::string::iterator> col(std::size_t j);

  [[nodiscard]] char at(std::size_t i, std::size_t j) const;
  [[nodiscard]] char &at(std::size_t i, std::size_t j);

  [[nodiscard]] std::size_t nrows() const noexcept { return n_rows; }
  [[nodiscard]] std::size_t ncols() const noexcept { return n_cols; }

private:
  std::size_t n_rows;
  std::size_t n_cols;
  std::string &text;

  static std::pair<std::size_t, std::size_t> dimensions(std::string_view input);
};

} // namespace views
} // namespace xmas