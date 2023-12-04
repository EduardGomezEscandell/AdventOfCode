#include "day04.hpp"
#include "xmaslib/log/log.hpp"

#include <algorithm>
#include <bits/ranges_algo.h>
#include <execution>
#include <format>
#include <numeric>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace {
std::pair<std::size_t, std::size_t> dimensions(std::string_view input) {
  const std::size_t ncols =
      1 + std::size_t(std::find(input.cbegin(), input.cend(), '\n') -
                      input.cbegin());
  const std::size_t nrows = input.size() / ncols;
  xlog::debug("Detected {} rows and {} columns", nrows, ncols);

  return std::make_pair(nrows, ncols);
}

} // namespace

std::int64_t Day04::part1() {
  const auto [nrows, ncols] = dimensions(this->input);

  std::vector<std::size_t> rows(nrows, 0);
  std::iota(rows.begin(), rows.end(), 0);

  return std::transform_reduce(
      std::execution::seq, rows.begin(), rows.end(), 0,
      std::plus<std::int64_t>{},
      [this, ncols](std::size_t const row) -> std::int64_t {
        auto begin = input.begin() + static_cast<std::ptrdiff_t>(row * ncols);
        auto end = begin + static_cast<std::ptrdiff_t>(ncols);

        auto it = std::find(begin, end, ':');
        if (it == end) {
          throw std::runtime_error(std::format("Row {} has no colon (:)", row));
        }

        std::vector<int> winners;

        // parse winners
        int n = 0;
        bool num = false;
        for (it = it + 1; it != end && *it != '|'; ++it) {
          if (*it == ' ') {
            if (num) {
              winners.push_back(n);
            }
            n = 0;
            num = false;
            continue;
          }

          num = true;
          n = 10 * n + (*it - '0');
        }

        std::ranges::sort(winners);

        // parse numbers we have
        std::int64_t hits = 0;
        n = 0;
        num = false;

        if (it != end) { // *it == '|'
          ++it;
        }
        for (; it != end; ++it) {
          if (*it == ' ' || *it == '\n') {
            if (num && std::ranges::binary_search(winners, n)) {
              ++hits;
            }
            num = false;
            n = 0;
            continue;
          }

          num = true;
          n = 10 * n + (*it - '0');
        }

        std::int64_t score = 0;
        if (hits > 0) {
          score = 1 << (hits - 1);
        }

        xlog::debug("Card {} has a score of {}", row + 1, score);
        return score;
      });
}

std::int64_t Day04::part2() { throw std::runtime_error("Not implemented"); }
