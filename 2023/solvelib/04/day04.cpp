#include "day04.hpp"
#include "xmaslib/log/log.hpp"

#include <algorithm>
#include <cstddef>
#include <execution>
#include <format>
#include <functional>
#include <numeric>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace {
[[nodiscard]] std::pair<std::size_t, std::size_t>
dimensions(std::string_view input) {
  const std::size_t ncols =
      1 + std::size_t(std::find(input.cbegin(), input.cend(), '\n') -
                      input.cbegin());
  const std::size_t nrows = input.size() / ncols;
  xlog::debug("Detected {} rows and {} columns", nrows, ncols);

  return std::make_pair(nrows, ncols);
}

[[nodiscard]] std::uint64_t card_hits(const std::size_t row,
                                     const std::string::const_iterator begin,
                                     const std::string::const_iterator end) {
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
  std::uint64_t hits = 0;
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

  return hits;
}

} // namespace

void Day04::load() {
  xmas::solution::load();

  // Trailing newline is necessary
  if (this->input.back() != '\n') {
    this->input.push_back('\n');
  }
}

std::uint64_t Day04::part1() {
  const auto [nrows, ncols] = dimensions(this->input);

  std::vector<std::size_t> rows(nrows, 0);
  std::iota(rows.begin(), rows.end(), 0);

  return std::transform_reduce(
      std::execution::par_unseq, rows.begin(), rows.end(), 0u,
      std::plus{},
      [this, ncols](std::size_t const row) -> std::uint64_t {
        const auto begin =
            input.cbegin() + static_cast<std::ptrdiff_t>(row * ncols);
        const auto end = begin + static_cast<std::ptrdiff_t>(ncols);

        const auto hits = card_hits(row, begin, end);

        std::uint64_t score = 0;
        if (hits > 0) {
          score = 1 << (hits - 1);
        }

        xlog::debug("Card {} has a score of {}", row + 1, score);
        return score;
      });
}

std::uint64_t Day04::part2() {
  const auto [nrows, ncols] = dimensions(this->input);

  std::vector<std::size_t> rows(nrows, 0);
  std::iota(rows.begin(), rows.end(), 0);

  // Parse in parallel, compute how many hits per card
  std::vector<std::int64_t> hits(nrows);
  std::transform(rows.cbegin(), rows.cend(), hits.begin(),
                 [this, ncols](std::size_t const row) -> std::uint64_t {
                   const auto begin = input.cbegin() +
                                      static_cast<std::ptrdiff_t>(row * ncols);
                   const auto end = begin + static_cast<std::ptrdiff_t>(ncols);
                   return card_hits(row, begin, end);
                 });

  // Must not be parallel because we overwrite copies
  std::vector<std::uint64_t> copies(nrows, 1);
  std::transform(rows.begin(), rows.end(), copies.begin(),
                 [&hits, &copies](std::size_t const row) -> std::uint64_t {
                   const auto nhits = hits[row];
                   const auto ncopies = copies[row];

                   xlog::debug("Game {}: there are {} copies with {} hits each",
                               row + 1, ncopies, nhits);

                   // Add copies of following cards
                   auto mbegin =
                       copies.begin() + static_cast<std::ptrdiff_t>(row) + 1;
                   auto mend = mbegin + nhits;
                   std::transform(mbegin, mend, mbegin,
                                  [ncopies](auto &m) { return m + ncopies; });

                   return ncopies;
                 });

  // std::ranges::reduce does not exist >:(
  return std::reduce(copies.begin(), copies.end());
}
