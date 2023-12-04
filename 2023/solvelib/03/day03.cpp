#include "day03.hpp"

#include "xmaslib/log/log.hpp"
#include <algorithm>
#include <cstddef>
#include <execution>
#include <functional>
#include <numeric>
#include <vector>

constexpr bool isnum(char ch) { return ch >= '0' && ch <= '9'; }

std::int64_t Day03::part1() {
  // Trailing newline is necessary
  if (this->input.back() != '\n') {
    this->input.push_back('\n');
  }

  const std::size_t ncols =
      1 + std::size_t(std::find(input.cbegin(), input.cend(), '\n') -
                      input.cbegin());
  const std::size_t nrows = this->input.size() / ncols;
  xlog::debug("Detected {} rows and {} columns", nrows, ncols);

  // Make a mask labeling all symbol-adjacent cells
  // Cannot use vector<bool> because writing into nearby entries is
  // thread-unsafe.
  std::vector<std::int8_t> mask(this->input.size(), 0);
  {
    // Detect symbols
    std::vector<std::int8_t> symbols(this->input.size(), 0);
    std::transform(std::execution::par_unseq, this->input.cbegin(),
                   this->input.cend(), symbols.begin(), [](char ch) {
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
    std::transform(std::execution::par_unseq, iota.cbegin(), iota.cend(),
                   mask.begin(),
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
                     if (!leftmost && symbols[idx - 1]==1)
                       return 1;
                     if (!rightmost && symbols[idx + 1]==1)
                       return 1;

                     // Top row
                     if (row > 0) {
                       const auto top = idx - ncols;
                       if (symbols[top]==1)
                         return 1;
                       if (!leftmost && symbols[top - 1]==1)
                         return 1;
                       if (!rightmost && symbols[top + 1]==1)
                         return 1;
                     }

                     // Bottom row
                     if (row < nrows - 1) {
                       const auto bot = idx + ncols;
                       if (symbols[bot]==1)
                         return 1;
                       if (!leftmost && symbols[bot - 1]==1)
                         return 1;
                       if (!rightmost && symbols[bot + 1]==1)
                         return 1;
                     }

                     return 0;
                   });
  }

  // Parse each row
  std::vector<std::size_t> rows(nrows, 0);
  std::iota(rows.begin(), rows.end(), 0);
  return std::transform_reduce(
      std::execution::par_unseq, rows.begin(), rows.end(), 0,
      std::plus<std::int64_t>{}, [ncols, this, &mask](std::size_t row) {
        auto begin = row * ncols;
        auto end = begin + ncols;

        std::int64_t total = 0;
        std::int64_t num = 0;
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
          num += input[idx] - '0';

          // Is it in contact?
          if (mask[idx] == 1)
            relevant = true;
        }

        return total;
      });
}

std::int64_t Day03::part2() { throw std::runtime_error("Not implemented"); }