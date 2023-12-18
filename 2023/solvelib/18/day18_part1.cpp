#include <algorithm>
#include <array>
#include <execution>
#include <format>
#include <functional>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <vector>

#include "day18.hpp"

#include "xmaslib/iota/iota.hpp"
#include "xmaslib/line_iterator/line_iterator.hpp"
#include "xmaslib/log/log.hpp"
#include "xmaslib/lazy_string/lazy_string.hpp"

namespace {

using lenght_t = int;

struct cell {
  bool filled;
};

struct coords {
  lenght_t row, col;
};

enum direction {
  up,
  down,
  right,
  left
};

struct instruction {
  direction dir;
  lenght_t len;

  static instruction parse(std::string_view ln) {
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

    lenght_t n = 0;
    for (auto it = ln.begin() + 2; it != ln.end(); ++it) {
      if (*it == ' ') {
        break;
      }
      n *= 10;
      n += static_cast<unsigned>(*it - '0');
    }

    return {d, n};
  }

  coords move(coords pos) const {
    switch (dir) {
    case up:
      return {.row = pos.row - len, .col = pos.col};
    case down:
      return {.row = pos.row + len, .col = pos.col};
    case left:
      return {.row = pos.row, .col = pos.col - len};
    case right:
      return {.row = pos.row, .col = pos.col + len};
    }
    throw std::runtime_error("Invalid instruction");
  }

  coords paint(std::vector<std::vector<cell>>& map, coords pos) const {
    auto end = move(pos);
    auto dr = (end.row - pos.row) / len;
    auto dc = (end.col - pos.col) / len;

    for (auto i = 0; i < len; ++i) {
      auto row = static_cast<std::size_t>(pos.row + dr * i);
      auto col = static_cast<std::size_t>(pos.col + dc * i);
      map[row][col].filled = true;
    }
    return end;
  }
};

auto print_map(std::vector<std::vector<cell>> const& map) {
  return xmas::lazy_string([&map]() {
    std::stringstream ss;
    for (std::size_t i = 0; i < map.size(); ++i) {
      for (std::size_t j = 0; j < map[i].size(); ++j) {
        ss << (map[i][j].filled ? '#' : '.');
      }
      ss << "\n";
    }
    return ss.str();
  });
}

std::vector<std::vector<cell>> build_map(std::string_view input) {
  xmas::views::linewise lines(input);

  // First pass: find the minimum and maximum coordinates
  std::array<coords, 2> boundaries{
    coords{0, 0},
    coords{0, 0}
  };

  coords pos{0, 0};
  std::vector<instruction> inst;
  for (std::string_view ln : lines) {
    inst.push_back(instruction::parse(ln));
    pos = inst.back().move(pos);
    boundaries[0].row = std::min(boundaries[0].row, pos.row);
    boundaries[0].col = std::min(boundaries[0].col, pos.col);
    boundaries[1].row = std::max(boundaries[1].row, pos.row);
    boundaries[1].col = std::max(boundaries[1].col, pos.col);
  }

  auto const nrows = static_cast<std::size_t>(boundaries[1].row - boundaries[0].row + 1);
  auto const ncols = static_cast<std::size_t>(boundaries[1].col - boundaries[0].col + 1);

  xlog::debug("The canvas streches from ({},{}) to ({},{})", boundaries[0].row, boundaries[0].col,
    boundaries[1].row, boundaries[1].col);
  std::vector map(nrows, std::vector<cell>(ncols));

  // Second pass: pain all the boundaries, with coordinates shifted so that the minima are (0,0)
  pos = {.row = -boundaries[0].row, .col = -boundaries[0].col};
  for (const instruction i : inst) {
    pos = i.paint(map, pos);
  }

  return map;
}

coords find_inside_point(std::vector<std::vector<cell>> const& map) {
  // There is the possibility that there is no inside: then we only count the walls
  coords fall_back{-1, -1};

  for (auto i : xmas::views::iota<std::size_t>(map.size())) {
    auto const& row = map[i];
    auto it = row.cbegin();
    while (true) {
      // Find wall
      it = std::find_if(it, row.cend(), [](cell c) { return c.filled; });
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
      if (it->filled) {
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
      return coords{
        .row = static_cast<lenght_t>(i),
        .col = static_cast<lenght_t>(it - row.cbegin()),
      };
    }
  }

  xlog::warning("Zero-size pool");
  return fall_back;
}

void flood_fill(std::vector<std::vector<cell>>& map, coords init) {
  map[std::size_t(init.row)][std::size_t(init.col)].filled = true;

  std::vector<coords> queue{init};
  while (!queue.empty()) {
    auto batch = std::move(queue);

    queue = {};
    queue.reserve(batch.size() * 4);

    for (auto c : batch) {
      auto candidates = {
        coords{c.row - 1, c.col    },
        coords{c.row + 1, c.col    },
        coords{c.row,     c.col - 1},
        coords{c.row,     c.col + 1}
      };

      for (auto c : candidates) {
        if (c.row < 0 || std::size_t(c.row) > map.size()) {
          continue;
        }
        if (c.col < 0 || std::size_t(c.col) > map[0].size()) {
          continue;
        }
        if (map[std::size_t(c.row)][std::size_t(c.col)].filled) {
          continue;
        }
        map[std::size_t(c.row)][std::size_t(c.col)].filled = true;
        queue.push_back(c);
      }
    }
  }
}

}

std::uint64_t Day18::part1() {
  auto map = build_map(input);
  xlog::debug("Map:\n{}", print_map(map));

  auto inside_point = find_inside_point(map);
  xlog::debug("Position ({},{}) is inside the pool", inside_point.row, inside_point.col);

  flood_fill(map, inside_point);
  xlog::debug("Map:\n{}", print_map(map));

  return std::transform_reduce(std::execution::par_unseq, map.cbegin(), map.cend(),
    std::uint64_t{0}, std::plus<std::uint64_t>{}, [](std::vector<cell> const& row) {
      return std::count_if(
        row.cbegin(), row.cend(), [](cell c) -> std::uint64_t { return c.filled; });
    });
}
