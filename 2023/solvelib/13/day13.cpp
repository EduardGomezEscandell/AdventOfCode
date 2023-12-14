#include "day13.hpp"

#include "xmaslib/line_iterator/line_iterator.hpp"
#include "xmaslib/log/log.hpp"
#include "xmaslib/stride/stride.hpp"
#include "xmaslib/view/view.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <execution>
#include <functional>
#include <numeric>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {

std::vector<std::string_view::iterator>
locate_block_begins(std::string_view input) {
  std::vector<std::string_view::iterator> block_begins{input.begin()};

  if (input.size() == 0) {
    return {};
  }

  for (auto it = input.begin(); it != input.end(); ++it) {
    it = std::adjacent_find(it, input.end(), [](char l, char r) -> bool {
      return l == '\n' && r == '\n';
    });
    if (it == input.end()) {
      break;
    }
    block_begins.push_back(it + 2);
  }

  block_begins.push_back(input.end() + 1);
  return block_begins;
}

std::pair<std::size_t, std::size_t> block_dimensions(std::string_view input) {
  const std::size_t ncols =
      1 + std::size_t(std::find(input.cbegin(), input.cend(), '\n') -
                      input.cbegin());
  const std::size_t nrows = input.size() / ncols;
  return std::make_pair(nrows, ncols);
}

template <typename Iterator>
void check_line_symmetry(std::vector<bool> &potential_axes, Iterator line_begin,
                         Iterator line_end) {
  assert(line_begin < line_end);
  assert(potential_axes.size() ==
         static_cast<std::size_t>(line_end - line_begin));

  auto v = xmas::view{line_begin, line_end};

  potential_axes[0] = false;
  for (std::size_t i = 1; i < potential_axes.size(); ++i) {
    if (!potential_axes[i]) {
      continue;
    }
    // Example: v=abccbaxyz, i=3
    auto left = v.take(i);            // abc
    auto right = v.drop(i).reverse(); // zyxabc

    auto shortest = std::min(left.size(), right.size()); // 3

    left.pop_front(left.size() - shortest);   // abc
    right.pop_front(right.size() - shortest); // abc

    auto it = std::mismatch(left.begin(), left.end(), right.begin());
    if (it.first != left.end()) {
      potential_axes[i] = false;
    }
  }
}

std::size_t check_block_symmetry(std::string_view block) {
  const auto [nrows, ncols] = block_dimensions(block);

  // Horizontal
  {
    const std::size_t width = ncols - 1; // -1 to skip the trailing endline
    std::vector<bool> potential_axes(width, true);

    for (std::size_t row = 0; row < nrows; ++row) {
      auto begin = block.begin() + static_cast<std::ptrdiff_t>(row * ncols);
      auto end = begin + static_cast<std::ptrdiff_t>(width);

      check_line_symmetry(potential_axes, begin, end);
    }

    auto it = std::ranges::find(potential_axes, true);
    if (it != potential_axes.end()) {
      xlog::debug("Horizontal symmetry @ {}", it - potential_axes.begin());
      return static_cast<std::uint64_t>(it - potential_axes.begin());
    }
  }

  // Vertical
  {
    std::vector<bool> potential_axes(nrows, true);

    for (std::size_t col = 0; col < ncols; ++col) {
      // We transpose the view by having a view with an iterator
      // where ++it means jump to next row
      auto begin = block.begin() + static_cast<std::ptrdiff_t>(col);
      auto end = begin + static_cast<std::ptrdiff_t>(nrows * ncols);
      auto v = xmas::views::strided(begin, end, ncols);

      check_line_symmetry(potential_axes, v.begin(), v.end());
    }

    auto it = std::ranges::find(potential_axes, true);
    if (it != potential_axes.end()) {
      xlog::debug("Vertical symmetry @ s{}", it - potential_axes.begin());
      return 100 * static_cast<std::uint64_t>(it - potential_axes.begin());
    }
  }

  throw std::runtime_error("Block has no symmetry");
}

} // namespace

void Day13::load() {
  xmas::solution::load();

  // Trailing newline is necessary
  if (this->input.back() != '\n') {
    this->input.push_back('\n');
  }
}

std::uint64_t Day13::part1() {
  auto block_begins = locate_block_begins(this->input);

  xlog::debug("Located {} blocks", block_begins.size() - 1);

  return std::transform_reduce(
      std::execution::seq, block_begins.begin(), block_begins.end() - 1,
      block_begins.begin() + 1, std::uint64_t{0}, std::plus<std::uint64_t>{},
      [](auto begin, auto begin_next) -> std::uint64_t {
        return check_block_symmetry(std::string_view{begin, begin_next - 1});
      });
}

std::uint64_t Day13::part2() { throw std::runtime_error("Not implemented"); }
