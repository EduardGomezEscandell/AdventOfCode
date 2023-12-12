#include "day11.hpp"

#include "xmaslib/log/log.hpp"
#include "xmaslib/iota/iota.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <execution>
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

struct galaxy {
  std::size_t row, col, best;
};


} // namespace

std::uint64_t Day11::part1() {
  const auto [nrows, ncols] = dimensions(this->input);

  std::vector<galaxy> galaxies;
  xmas::views::iota<std::size_t> iota(this->input.size());

  auto galaxies = std::transform_reduce(std::execution::par_unseq,
                                        iota.cbegin(),
                                        iota.cend(), )
}

std::uint64_t Day11::part2() { throw std::runtime_error("Not implemented"); }
