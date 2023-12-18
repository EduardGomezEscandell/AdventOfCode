#include "day03.hpp"

#include <algorithm>
#include <cstddef>
#include <execution>
#include <format>
#include <functional>
#include <numeric>
#include <utility>
#include <vector>

#include "xmaslib/log/log.hpp"
#include "xmaslib/solution/solution.hpp"

constexpr bool isnum(char ch) {
  return ch >= '0' && ch <= '9';
}

namespace {
std::pair<std::size_t, std::size_t> dimensions(std::string_view input) {
  const std::size_t ncols =
    1 + std::size_t(std::find(input.cbegin(), input.cend(), '\n') - input.cbegin());
  const std::size_t nrows = input.size() / ncols;
  xlog::debug("Detected {} rows and {} columns", nrows, ncols);

  return std::make_pair(nrows, ncols);
}
} // namespace

std::uint64_t Day03::part1() {
  const auto [nrows, ncols] = dimensions(this->input);

  // Make a mask labeling all symbol-adjacent cells
  // Cannot use vector<bool> because writing into nearby entries is
  // thread-unsafe.
  std::vector<std::int8_t> mask(this->input.size(), 0);
  {
    // Detect symbols
    std::vector<std::int8_t> symbols(this->input.size(), 0);
    std::transform(std::execution::par_unseq, this->input.cbegin(), this->input.cend(),
      symbols.begin(), [](char ch) {
        if (ch == '.')
          return 0;
        if (ch == '\n')
          return 0;
        if (ch >= '0' && ch <= '9')
          return 0;

        // Upper row
        return 1;
      });

    std::vector<std::size_t> iota(this->input.size(), 0);
    std::iota(iota.begin(), iota.end(), 0);

    // Expand the mask to cover symbol adjacency
    std::transform(std::execution::par_unseq, iota.cbegin(), iota.cend(), mask.begin(),
      [nrows, ncols, &symbols](std::size_t idx) -> std::int64_t {
        // Top row
        const auto row = idx / ncols;
        const auto col = idx % ncols;

        const bool leftmost = (col == 0);
        const bool rightmost = (col == ncols - 1);

        if (rightmost) {
          // Skip \n
          return 0;
        }

        // Same row
        if (!leftmost && symbols[idx - 1] == 1)
          return 1;
        if (!rightmost && symbols[idx + 1] == 1)
          return 1;

        // Top row
        if (row > 0) {
          const auto top = idx - ncols;
          if (symbols[top] == 1)
            return 1;
          if (!leftmost && symbols[top - 1] == 1)
            return 1;
          if (!rightmost && symbols[top + 1] == 1)
            return 1;
        }

        // Bottom row
        if (row < nrows - 1) {
          const auto bot = idx + ncols;
          if (symbols[bot] == 1)
            return 1;
          if (!leftmost && symbols[bot - 1] == 1)
            return 1;
          if (!rightmost && symbols[bot + 1] == 1)
            return 1;
        }

        return 0;
      });
  }

  // Parse each row
  std::vector<std::size_t> rows(nrows, 0);
  std::iota(rows.begin(), rows.end(), 0);
  return std::transform_reduce(std::execution::par_unseq, rows.begin(), rows.end(), 0u, std::plus{},
    [ncols, this, &mask](std::size_t row) {
      auto begin = row * ncols;
      auto end = begin + ncols;

      std::uint64_t total = 0;
      std::uint64_t num = 0;
      bool relevant = false;

      for (std::size_t idx = begin; idx != end; ++idx) {
        if (!isnum(input[idx])) {
          if (relevant) {
            total += num;
            xlog::debug("Row {}: Adding number {}", row, num);
          } else if (num != 0) {
            xlog::debug("Row {}: Ignoring number {}", row, num);
          }
          num = 0;
          relevant = false;
          continue;
        }

        // Parse next digit
        num *= 10;
        num += uint(input[idx] - '0');

        // Is it in contact?
        if (mask[idx] == 1)
          relevant = true;
      }

      return total;
    });
}

std::uint64_t Day03::part2() {
  const auto [nrows, ncols] = dimensions(this->input);

  std::vector<std::size_t> rows(nrows, 0);
  std::iota(rows.begin(), rows.end(), 0);

  // Build a matrix where each entry is the number that digit belongs to
  std::vector numbers(nrows, std::vector(ncols, std::int64_t(-1)));
  std::for_each(std::execution::par_unseq, rows.begin(), rows.end(),
    [ncols, this, &numbers](const std::size_t row) {
      auto row_begin = row * ncols;
      std::size_t num_begin = 0;

      std::int64_t num = 0;

      for (std::size_t col = 0; col < ncols; ++col) {
        const auto idx = row_begin + col;

        if (!isnum(input[idx])) {
          if (col - num_begin != 0) {
            xlog::debug("Found number {} in row {}, cols [{}, {})", num, row, num_begin, col);
            std::fill(numbers[row].begin() + std::ptrdiff_t(num_begin),
              numbers[row].begin() + std::ptrdiff_t(col), num);
          }

          num = 0;
          num_begin = col + 1;
          continue;
        }

        // Parse next digit
        num *= 10;
        num += input[idx] - '0';
      }
    });

  // Scan for gears
  std::vector<std::size_t> iota(nrows * ncols, 0);
  std::iota(iota.begin(), iota.end(), 0);
  return std::transform_reduce(std::execution::par_unseq, iota.begin(), iota.end(), 0u, std::plus{},
    [this, nrows, ncols, &numbers](const std::size_t idx) -> std::uint64_t {
      if (this->input[idx] != '*') {
        return 0;
      }

      const auto row = idx / ncols;
      const auto col = idx % ncols;

      std::vector<std::uint64_t> neighbours;
      neighbours.reserve(5);

      const bool leftmost = (col == 0);
      const bool rightmost = (col == ncols - 1);

      // Side neighbours
      if (!leftmost && numbers[row][col - 1] != -1) {
        neighbours.emplace_back(numbers[row][col - 1]);
      }
      if (!rightmost && numbers[row][col + 1] != -1) {
        neighbours.emplace_back(numbers[row][col + 1]);
      }

      // Up neighbours
      if (row != 0) {
        auto r = numbers[row - 1];
        if (r[col] != -1) {
          // Either a number on top
          neighbours.emplace_back(r[col]);
        } else {
          // Or numbers on top-left and top-right
          if (!leftmost && r[col - 1] != -1) {
            neighbours.emplace_back(r[col - 1]);
          }
          if (!rightmost && r[col + 1] != -1) {
            neighbours.emplace_back(r[col + 1]);
          }
        }
      }

      // Down neighbours
      if (row + 1 != nrows) {
        auto r = numbers[row + 1];
        if (r[col] != -1) {
          // Either a number below
          neighbours.emplace_back(r[col]);
        } else {
          // Or numbers below-left and below-right
          if (!leftmost && r[col - 1] != -1) {
            neighbours.emplace_back(r[col - 1]);
          }
          if (!rightmost && r[col + 1] != -1) {
            neighbours.emplace_back(r[col + 1]);
          }
        }
      }

      if (auto sz = neighbours.size(); sz != 2) {
        xlog::debug("Gear in row {} has {} neighbour{}", row, sz, sz > 1 ? "s" : "");
        return 0;
      }

      xlog::debug("Gear in row {} has neighbours {} and {}", row, neighbours[0], neighbours[1]);
      return neighbours[0] * neighbours[1];
    });
}