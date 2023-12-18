#include "day11.hpp"

#include "xmaslib/iota/iota.hpp"
#include "xmaslib/log/log.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <execution>
#include <map>
#include <string_view>
#include <utility>
#include <vector>

namespace {

struct coords {
  std::size_t row, col;
};

std::uint64_t absolute_difference(std::uint64_t a, std::uint64_t b) {
  return (a > b) ? (a - b) : (b - a);
}

[[nodiscard]] std::pair<std::size_t, std::size_t>
dimensions(std::string_view input) {
  const std::size_t ncols =
      1 + std::size_t(std::find(input.cbegin(), input.cend(), '\n') -
                      input.cbegin());
  const std::size_t nrows = input.size() / ncols;
  xlog::debug("Detected {} rows and {} columns", nrows, ncols);

  return std::make_pair(nrows, ncols);
}

auto parse_and_expand_universe(std::string_view input,
                               std::size_t expansion_rate) {
  const auto [nrows, ncols] = dimensions(input);

  // Find galaxies and expand rows
  std::vector<coords> galaxies;
  std::map<std::size_t, std::size_t> column_mapping;
  {
    std::size_t row = 0;
    for (std::size_t line = 0; line < nrows; ++line) {
      bool empty = true;
      for (std::size_t col = 0; col < ncols - 1; ++col) {
        const std::size_t i = line * ncols + col;
        if (input[i] == '#') {
          galaxies.push_back({row, col});
          column_mapping[col] = col;
          empty = false;
        }
      }
      if (empty) {
        row += expansion_rate;
      } else {
        ++row;
      }
    };
  }

  // Create mapping from original to expanded columns
  {
    std::size_t empties = 0;
    std::ptrdiff_t prev_row = -1;
    for (auto &map : column_mapping) {
      auto row = std::ptrdiff_t(map.first);
      empties += std::size_t(row - prev_row - 1);
      prev_row = row;

      map.second = map.first + empties * (expansion_rate - 1);
    }
  }

#ifndef NDEBUG
  for (auto map : column_mapping) {
    xlog::debug("Mapping columns {}->{}", map.first, map.second);
  }
#endif

  // Expand columns
  std::for_each(
      std::execution::par_unseq, galaxies.begin(), galaxies.end(),
      [&column_mapping](coords &g) { g.col = column_mapping.at(g.col); });

#ifndef NDEBUG
  for (auto i : xmas::views::iota(galaxies.size())) {
    xlog::debug("Galaxy {}:  {},{}", i, galaxies[i].row, galaxies[i].col);
  }
#endif

  return galaxies;
}

}  // namespace

std::uint64_t Day11::solve(std::size_t expansion_rate) const {
  auto galaxies = parse_and_expand_universe(input, expansion_rate);

  // Compute distances
  std::size_t n = galaxies.size();
  std::vector<std::uint64_t> distances((n * (n - 1)) / 2);

  xmas::views::iota<std::size_t> iota(n);
  std::for_each(std::execution::par_unseq, iota.begin(), iota.end(),
                [&galaxies, &distances](std::size_t i) {
                  auto &g1 = galaxies[i];
                  for (std::size_t j = i + 1; j != galaxies.size(); ++j) {
                    auto &g2 = galaxies[j];

                    // Taxicab distance
                    std::uint64_t vdistance =
                        absolute_difference(g2.row, g1.row);
                    std::uint64_t hdistance =
                        absolute_difference(g2.col, g1.col);
                    std::uint64_t d = vdistance + hdistance;

                    xlog::debug("Distance between {} and {} is {}", i, j, d);
                    std::size_t dest = (j * (j - 1)) / 2 + i; // <3 Gauss
                    distances[dest] = d;
                  }
                });

  return std::reduce(std::execution::unseq, distances.begin(), distances.end(),
                     std::uint64_t{0}, std::plus<std::uint64_t>{});
}

std::uint64_t Day11::part1() { return solve(2); }
std::uint64_t Day11::part2() { return solve(1'000'000); }
