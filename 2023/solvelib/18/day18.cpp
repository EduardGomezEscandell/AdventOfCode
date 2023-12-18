#include "day18.hpp"

#include "xmaslib/functional/functional.hpp"
#include "xmaslib/log/log.hpp"
#include "xmaslib/parsing/parsing.hpp"
#include "xmaslib/line_iterator/line_iterator.hpp"
#include "xmaslib/lazy_string/lazy_string.hpp"
#include "xmaslib/iota/iota.hpp"

#include <algorithm>
#include <cmath>
#include <execution>
#include <format>
#include <functional>
#include <numeric>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace {

// Actual x,y coordinates
using length_t = std::int64_t;

// Blocks (rows, columns)
using block_t = long;

struct cell {
  bool filled;
};

enum direction {
  right = '0',
  down = '1',
  left = '2',
  up = '3',
};

struct coords {
  length_t y, x;
};

struct blocks {
  block_t row, col;
};

template <typename T>
auto print_vector(std::vector<T> const& v) {
  return xmas::lazy_string{[&]() {
    std::stringstream ss;
    for (std::size_t i = 0; i < v.size(); ++i) {
      ss << std::format(" {:>2}:{:<7}", i, v[i]);
    }
    return ss.str();
  }};
}

/*
The strategy is to divide the domain in blocks.
Note that the path has its own block, because it is not zero-width.

We group up multiple values of X into columns. Every point along the path has its own row and
column.

Let's use a segment of the path
R 5
D 4

On the left, a matrix showing where the path goes. On the right, the partition of the matrix
in each section. Note that sections are rectangular.
     0 1 2 3 4       0 1 2 3 4
   0 # # # # #   > 0 A B B B C
   1 . . . . #   > 1 D E E E F
   2 . . . . #   > 2 D E E E F
   3 . . . . #   > 3 G H H H I

Every section is aligned to a grid of rows and columns:
     Column   Range             Row  Range
       0        [0,1)            0    [0,1)
       1        [1,3)            1    [1,2)
       2        [3,4)            2    [2,3)

We can store this in two vectors:
C: {0,1,3,4}    // Columns
R: {0,1,2,3}    // Rows

Hence, column i has range [ C[i] , C[i+1] ).
*/

struct canvas {
  std::vector<length_t> R;
  std::vector<length_t> C;
  std::vector<std::vector<bool>> visited;

  auto to_lazy_string() const {
    return xmas::lazy_string([this]() {
      std::stringstream ss;
      for (std::size_t i = 0; i < visited.size(); ++i) {
        for (std::size_t j = 0; j < visited[i].size(); ++j) {
          ss << (visited[i][j] ? '#' : '.');
        }
        ss << "\n";
      }
      return ss.str();
    });
  }

  blocks find_inside_point() const {
    // There is the possibility that there is no inside: then we only count the walls
    blocks fall_back{-1, -1};

    for (auto i : xmas::views::iota<std::size_t>(visited.size())) {
      auto const& row = visited[i];
      auto it = row.cbegin();
      while (true) {
        // Find wall
        it = std::find(it, row.cend(), true);
        if (it == row.cend()) {
          throw std::runtime_error("Empty row");
        }

        // Check if next cell is empty (aka the inside of the pool)
        ++it;

        // Single wall along the edge. Weird but OK
        if (it == row.cend()) {
          break;
        }

        // Two consecutive walls: unfortunate
        if (*it) {
          // Note: we'd need something smarter against certain adversarial inputs,
          // but this seems good enough for AoC.
          //
          // This input would break us:
          // .###
          // ##.#
          // .###
          //
          // ^ See that every line is preceded by multiple walls
          break;
        }

        // Success!
        return blocks{
          .row = static_cast<block_t>(i),
          .col = static_cast<block_t>(it - row.cbegin()),
        };
      }
    }

    xlog::warning("Zero-size pool");
    return fall_back;
  }

  void flood_fill(blocks init) {
    visited[std::size_t(init.row)][std::size_t(init.col)] = true;

    std::vector<blocks> queue{init};
    while (!queue.empty()) {
      auto batch = std::move(queue);

      queue = {};
      queue.reserve(batch.size() * 4);

      for (auto c : batch) {
        auto candidates = {
          blocks{c.row - 1, c.col    },
          blocks{c.row + 1, c.col    },
          blocks{c.row,     c.col - 1},
          blocks{c.row,     c.col + 1}
        };

        for (auto c : candidates) {
          if (c.row < 0 || std::size_t(c.row) > visited.size()) {
            continue;
          }
          if (c.col < 0 || std::size_t(c.col) > visited[0].size()) {
            continue;
          }
          if (visited[std::size_t(c.row)][std::size_t(c.col)]) {
            continue;
          }
          visited[std::size_t(c.row)][std::size_t(c.col)] = true;
          queue.push_back(c);
        }
      }
    }
  }
};

block_t len_to_block(length_t p, std::vector<length_t> const& mapping) {
  auto it = std::find_if(mapping.begin(), mapping.end(), xmas::equals(p));
  if (it == mapping.end()) {
    throw std::runtime_error(std::format("Coordinate past end of the range"));
  }

  return static_cast<block_t>(it - mapping.begin());
}

blocks coords_to_blocks(coords p, canvas const& map) {
  return blocks{.row = len_to_block(p.y, map.R), .col = len_to_block(p.x, map.C)};
}

struct instruction {
  direction dir;
  length_t len;

  static instruction parse_part1(std::string_view ln) {
    const auto d = [ln]() {
      switch (ln.front()) {
      case 'U':
        return up;
      case 'D':
        return down;
      case 'R':
        return right;
      case 'L':
        return left;
      default:
        throw std::runtime_error(std::format("Line does not contain direction: {}", ln.front()));
      }
    }();

    if (ln.size() < 3) {
      throw std::runtime_error(std::format("Line too short: {}", ln.front()));
    }

    length_t n = 0;
    for (auto it = ln.begin() + 2; it != ln.end(); ++it) {
      if (*it == ' ') {
        break;
      }
      n *= 10;
      n += static_cast<unsigned>(*it - '0');
    }

    return {d, n};
  }

  static instruction parse_part2(std::string_view sv) {
    auto it = std::ranges::find(sv, '#');
    if (it == sv.end()) {
      throw std::runtime_error(std::format("Line has no #: {}", sv));
    }

    if ((it + 6) >= sv.end()) {
      throw std::runtime_error(std::format("Line too short after #: {}", sv));
    }

    const auto n = xmas::parse_hex<length_t>(std::string_view{it + 1, it + 6});
    const auto d = static_cast<direction>(*(it + 6));

    return {d, n};
  }

  coords move(coords pos) const {
    switch (dir) {
    case up:
      return {.y = pos.y - len, .x = pos.x};
    case down:
      return {.y = pos.y + len, .x = pos.x};
    case left:
      return {.y = pos.y, .x = pos.x - len};
    case right:
      return {.y = pos.y, .x = pos.x + len};
    }
    throw std::runtime_error("Invalid instruction");
  }

  coords paint(canvas& map, coords pos) const {
    const auto start = coords_to_blocks(pos, map);
    const auto out = move(pos);
    const auto end = coords_to_blocks(out, map);

    auto dr = (end.row - start.row);
    auto dc = (end.col - start.col);
    const auto L = std::max(std::abs(dr), std::abs(dc));
    dr /= L;
    dc /= L;

    for (auto i = 0; i <= L; ++i) {
      const auto row = static_cast<std::size_t>(start.row + dr * i);
      const auto col = static_cast<std::size_t>(start.col + dc * i);
      map.visited[row][col] = true;
    }
    return out;
  }
};

canvas build_canvas(std::string_view input, instruction parseFunc(std::string_view)) {
  xmas::views::linewise lines(input);

  std::vector<instruction> instr;
  std::set<length_t> rows{0};
  std::set<length_t> cols{0};

  coords pos{0, 0};
  for (auto ln : lines) {
    auto i = parseFunc(ln);
    instr.push_back(i);
    pos = i.move(pos);

    rows.insert(pos.y);
    rows.insert(pos.y + 1);

    cols.insert(pos.x);
    cols.insert(pos.x + 1);
  }

  auto map = canvas{.R = std::vector<length_t>(rows.begin(), rows.end()),
    .C = std::vector<length_t>(cols.begin(), cols.end()),
    .visited = std::vector(rows.size() - 1, std::vector(cols.size() - 1, false))};

  pos = {0, 0};
  for (auto i : instr) {
    pos = i.paint(map, pos);
  }

  return map;
}

std::uint64_t solve(std::string_view input, instruction parseFunc(std::string_view)) {
  auto map = build_canvas(input, parseFunc);

  xlog::debug("Canvas spans from ({},{}) to ({},{})", map.R.front(), map.C.front(), map.R.back(),
    map.C.back());
  xlog::debug("Canvas has {} rows and {} columns", map.R.size() - 1, map.C.size() - 1);
  xlog::debug("Map:\n{}", map.to_lazy_string());

  auto inside_point = map.find_inside_point();
  xlog::debug("Position ({},{}) is inside the pool", inside_point.row, inside_point.col);

  map.flood_fill(inside_point);
  xlog::debug("Map:\n{}", map.to_lazy_string());

  std::vector<std::uint64_t> row_heights(map.visited.size());
  std::vector<std::uint64_t> column_widths(map.visited[0].size());

  std::transform(std::execution::unseq, map.R.begin(), map.R.end() - 1, map.R.begin() + 1,
    row_heights.begin(), std::minus<std::uint64_t>{});
  std::transform(std::execution::unseq, map.C.begin(), map.C.end() - 1, map.C.begin() + 1,
    column_widths.begin(), std::minus<std::uint64_t>{});

  auto rows = xmas::views::iota<std::size_t>(map.visited.size());
  // Row-wise reduction
  return std::transform_reduce(std::execution::par_unseq, rows.begin(), rows.end(),
    std::uint64_t{0}, std::plus<std::uint64_t>{}, [&](std::size_t row) {
      // Column-wise reduction
      return row_heights[row] * std::transform_reduce(map.visited[row].begin(),
                                  map.visited[row].end(), column_widths.begin(), std::uint64_t{0},
                                  std::plus<std::uint64_t>{}, std::multiplies<std::uint64_t>{});
    });
}

}

std::uint64_t Day18::part1() {
  return solve(this->input, instruction::parse_part1);
}

std::uint64_t Day18::part2() {
  return solve(this->input, instruction::parse_part2);
}