#include "day13.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <execution>
#include <functional>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>

#include "xmaslib/functional/functional.hpp"
#include "xmaslib/line_iterator/line_iterator.hpp"
#include "xmaslib/log/log.hpp"
#include "xmaslib/stride/stride.hpp"
#include "xmaslib/view/view.hpp"

namespace {

std::vector<std::string_view::iterator> locate_block_begins(std::string_view input) {
  std::vector<std::string_view::iterator> block_begins{input.begin()};

  if (input.size() == 0) {
    return {};
  }

  for (auto it = input.begin(); it != input.end(); ++it) {
    it = std::adjacent_find(
      it, input.end(), [](char l, char r) -> bool { return l == '\n' && r == '\n'; });
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
    1 + std::size_t(std::find(input.cbegin(), input.cend(), '\n') - input.cbegin());
  const std::size_t nrows = input.size() / ncols;
  return std::make_pair(nrows, ncols);
}

template <typename Iterator>
void check_line_symmetry(
  std::vector<bool>& potential_axes, Iterator line_begin, Iterator line_end) {
  assert(line_begin < line_end);
  assert(potential_axes.size() == static_cast<std::size_t>(line_end - line_begin));

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

std::vector<std::size_t> check_block_horizontal_symmetries(
  std::string_view block, std::size_t nrows, std::size_t ncols) {
  const std::size_t width = ncols - 1; // -1 to skip the trailing endline
  std::vector<bool> potential_axes(width, true);

  for (std::size_t row = 0; row < nrows; ++row) {
    auto begin = block.begin() + static_cast<std::ptrdiff_t>(row * ncols);
    auto end = begin + static_cast<std::ptrdiff_t>(width);

    check_line_symmetry(potential_axes, begin, end);
  }

  std::vector<std::size_t> symmetries;
  for (std::size_t i = 0; i < potential_axes.size(); ++i) {
    if (potential_axes[i]) {
      symmetries.push_back(i);
    }
  }
  return symmetries;
}

std::vector<std::size_t> check_block_vertical_symmetries(
  std::string_view block, std::size_t nrows, std::size_t ncols) {
  std::vector<bool> potential_axes(nrows, true);

  for (std::size_t col = 0; col < ncols; ++col) {
    // We transpose the block by having a view with an iterator
    // where ++it means jump to next row.
    auto begin = block.begin() + static_cast<std::ptrdiff_t>(col);
    auto end = begin + static_cast<std::ptrdiff_t>(nrows * ncols);
    auto v = xmas::views::strided(begin, end, ncols);

    check_line_symmetry(potential_axes, v.begin(), v.end());
  }

  std::vector<std::size_t> symmetries;
  for (std::size_t i = 0; i < potential_axes.size(); ++i) {
    if (potential_axes[i]) {
      symmetries.push_back(i);
    }
  }
  return symmetries;
}

std::pair<std::uint64_t, std::uint64_t> solve_block(
  std::string_view block, std::size_t nrows, std::size_t ncols) {
  auto h = check_block_horizontal_symmetries(block, nrows, ncols);
  if (h.size() != 0) {
    return {h.front(), 0};
  }

  auto v = check_block_vertical_symmetries(block, nrows, ncols);
  if (v.size() != 0) {
    return {0, v.front()};
  }

  xlog::warning("block with no possible symmetry");
  return {0, 0};
}

char flip(char c) {
  if (c == '#')
    return '.';
  if (c == '.')
    return '#';
  return c;
}

} // namespace

std::uint64_t Day13::part1() {
  auto block_begins = locate_block_begins(this->input);

  xlog::debug("Located {} blocks", block_begins.size() - 1);

  return std::transform_reduce(std::execution::par_unseq, block_begins.begin(),
    block_begins.end() - 1, block_begins.begin() + 1, std::uint64_t{0}, std::plus<std::uint64_t>{},
    [](auto begin, auto begin_next) {
      std::string_view block{begin, begin_next - 1};
      const auto [nrows, ncols] = block_dimensions(block);
      auto p1 = solve_block(block, nrows, ncols);
      return p1.first + 100 * p1.second;
    });
}

std::uint64_t Day13::part2() {
  auto block_begins = locate_block_begins(this->input);

  xlog::debug("Located {} blocks", block_begins.size() - 1);

  return std::transform_reduce(std::execution::par_unseq, block_begins.begin(),
    block_begins.end() - 1, block_begins.begin() + 1, std::uint64_t{0}, std::plus<std::uint64_t>{},
    [](auto begin, auto begin_next) -> std::uint64_t {
      // Blocks are relatively small (smaller than 20x20) so we can
      // brute-force it
      std::string block{begin, begin_next - 1};
      const auto [nrows, ncols] = block_dimensions(block);
      auto part1 = solve_block(block, nrows, ncols);

      for (char& c : block) {
        if (c == '\n') {
          continue;
        }

        c = flip(c);

        auto sym = check_block_horizontal_symmetries(block, nrows, ncols);
        auto it = std::ranges::find_if_not(sym, xmas::equals(part1.first));
        if (it != sym.end()) {
          return *it;
        }

        sym = check_block_vertical_symmetries(block, nrows, ncols);
        it = std::ranges::find_if_not(sym, xmas::equals(part1.second));
        if (it != sym.end()) {
          return 100 * (*it);
        }

        c = flip(c);
      }

      xlog::warning("block with no possible symmetry");
      return 0;
    });
}
