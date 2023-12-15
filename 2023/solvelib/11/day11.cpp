#include "day11.hpp"

#include "xmaslib/iota/iota.hpp"
#include "xmaslib/log/log.hpp"
#include "xmaslib/matrix/csc.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <execution>
#include <functional>
#include <numeric>
#include <stdexcept>
#include <string_view>
#include <utility>
#include <vector>

namespace {

using distance_t = std::uint64_t;

[[nodiscard]] std::pair<std::size_t, std::size_t>
dimensions(std::string_view input) {
  const std::size_t ncols =
      1 + std::size_t(std::find(input.cbegin(), input.cend(), '\n') -
                      input.cbegin());
  const std::size_t nrows = input.size() / ncols;
  xlog::debug("Detected {} rows and {} columns", nrows, ncols);

  return std::make_pair(nrows, ncols);
}

// Compresses a matrix as a vector of vectors. There is an entry for each row.
// At every row, there is a list of all the columns that fulfil the predicate
[[nodiscard]] xmas::csc_matrix<std::size_t> parse_input_and_expand(std::string_view input) {
  const auto [nrows, ncols] = dimensions(input);

  // Excessively large matrix, we'll shrink it in the end
  xmas::csc_matrix<std::size_t> matrix(2 * nrows,
                                       2 * (ncols - 1)); // -1 to skip "\n"

  xmas::views::iota<std::size_t> rows(nrows);
  std::size_t id = 0;
  std::size_t row = 0;
  for (std::size_t line = 0; line < nrows; ++line) {
    std::vector<std::size_t> columns;
    std::vector<std::size_t> indeces;
    for (std::size_t col = 0; col < ncols - 1; ++col) {
      const std::size_t i = line * ncols + col;
      if (input[i] == '#') {
        columns.push_back(col);
        indeces.push_back(id++);
      }
    }
    if (columns.size() == 0) {
      row += 2;
    } else {
      matrix.init_row(row++, columns, indeces);
    }
  };

  std::for_each(matrix.begin(), matrix.end(), [](auto g) {
    xlog::debug("Galaxy {}:  {},{}", g.value, g.row, g.col);
  });

  // Update columns according to empty cols

  // Create a mapping col -> new col
  xmas::views::iota<std::size_t> iota(ncols);
  std::vector<std::size_t> true_column(ncols);

  auto col_density = matrix.col_density();
  std::transform_exclusive_scan(
      std::execution::par_unseq, iota.begin(), iota.end(), true_column.begin(),
      std::size_t{0}, std::plus<std::size_t>{},
      [&](std::size_t col) { return (col_density[col] ? 1u : 2u); });

  // Map columns to new coordinates
  std::for_each(std::execution::par_unseq, matrix.raw_columns().begin(),
                matrix.raw_columns().end(),
                [&true_column](std::size_t &col) { col = true_column[col]; });

  matrix.shrink();
  return matrix;
}

struct coords {
  std::size_t row, col;

  bool operator==(coords const &other) const noexcept {
    return row == other.row && col == other.col;
  }
};

} // namespace

void Day11::load() {
  xmas::solution::load();

  // Trailing newline is necessary
  if (this->input.back() != '\n') {
    this->input.push_back('\n');
  }
}

std::uint64_t Day11::part1() {

  // Compress matrix
  auto galaxies = parse_input_and_expand(input);

  // Compute distances
  std::size_t n = galaxies.size();
  std::vector<distance_t> distances;
  distances.reserve((n * (n - 1)) / 2);
  for (auto it = galaxies.begin(); it != galaxies.end(); ++it) {
    auto g1 = *it;
    std::size_t i = g1.value;
    for (auto jt = std::next(it); jt != galaxies.end(); ++jt) {
      auto g2 = *jt;
      std::size_t j = g2.value;

      // Taxicab distance
      distance_t vdistance = g2.row - g1.row;
      distance_t hdistance = /* abs */
          (g2.col > g1.col) ? (g2.col - g1.col) : (g1.col - g2.col);
      distance_t d = vdistance + hdistance;

      xlog::debug("Distance between {} and {} is {}", i, j, d);
      distances.push_back(d);
    }
  }

  return std::reduce(std::execution::unseq, distances.begin(), distances.end(),
                     distance_t{0}, std::plus<distance_t>{});
}

std::uint64_t Day11::part2() { throw std::runtime_error("Not implemented"); }
