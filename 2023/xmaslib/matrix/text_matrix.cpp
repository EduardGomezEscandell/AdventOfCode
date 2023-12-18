#include "text_matrix.hpp"
#include "../log/log.hpp"

#include <algorithm>
#include <cstddef>
#include <string>
#include <utility>

namespace xmas {
namespace views {

std::pair<std::size_t, std::size_t> text_matrix::dimensions(
  std::string_view input) {
  const std::size_t ncols =
    1 +
    std::size_t(std::find(input.cbegin(), input.cend(), '\n') - input.cbegin());
  const std::size_t nrows = input.size() / ncols;
  xlog::debug("Detected {} rows and {} columns", nrows, ncols);

  return std::make_pair(nrows, ncols);
}

text_matrix::text_matrix(std::string& data) : text(data) {
  auto [nrows, ncols] = dimensions(text);
  this->n_rows = nrows;
  this->n_cols = ncols - 1; // Skip \n
}

view<std::string::iterator> text_matrix::row(std::size_t i) {
  assert(i < n_rows);
  const std::size_t pos = i * (n_rows + 1);
  return {text.begin() + static_cast<std::ptrdiff_t>(pos),
    text.begin() + static_cast<std::ptrdiff_t>(pos + n_cols)};
}

strided<std::string::iterator> text_matrix::col(std::size_t j) {
  assert(j < n_cols);
  return {
    text.begin() + static_cast<std::ptrdiff_t>(j), text.end(), n_cols + 1};
}

char text_matrix::at(std::size_t i, std::size_t j) const {
  assert(i < n_rows);
  assert(j < n_cols);
  return text[i * (n_cols + 1) + j];
}

char& text_matrix::at(std::size_t i, std::size_t j) {
  assert(i < n_rows);
  assert(j < n_cols);
  return text[i * (n_cols + 1) + j];
}

} // namespace views
} // namespace xmas