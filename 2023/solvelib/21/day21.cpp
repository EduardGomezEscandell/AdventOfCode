#include "day21.hpp"

#include "xmaslib/log/log.hpp"
#include "xmaslib/matrix/matrix.hpp"
#include "xmaslib/matrix/text_matrix.hpp"
#include "xmaslib/lazy_string/lazy_string.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <execution>
#include <functional>
#include <iterator>
#include <limits>
#include <numeric>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace {

struct coords {
  std::size_t row, col;
};

}

namespace {

coords find_start(xmas::views::text_matrix map) {
  auto it = std::ranges::find(map.data(), 'S');
  if (it == map.data().end()) {
    throw std::runtime_error("Map contains no starting point");
  }

  const auto delta = static_cast<std::size_t>(it - map.data().begin());
  return {
    delta / (map.ncols() + 1), // Counting endlines
    delta % (map.ncols() + 1), // Counting endlines
  };
}

constexpr auto sentinel = std::numeric_limits<std::size_t>::max();

void bfs(xmas::views::text_matrix map, xmas::matrix<std::size_t>& distances, std::size_t d,
  coords pos, std::back_insert_iterator<std::vector<coords>> enqueuer) {
  if (distances[pos.row][pos.col] != sentinel) {
    return;
  }

  distances[pos.row][pos.col] = d;

  if (pos.row != 0) {
    coords c{pos.row - 1, pos.col};
    if (map.at(c.row, c.col) != '#') {
      *enqueuer = c;
    }
  }
  if (pos.row + 1 < map.nrows()) {
    coords c{pos.row + 1, pos.col};
    if (map.at(c.row, c.col) != '#') {
      *enqueuer = c;
    }
  }
  if (pos.col != 0) {
    coords c{pos.row, pos.col - 1};
    if (map.at(c.row, c.col) != '#') {
      *enqueuer = c;
    }
  }
  if (pos.col + 1 < map.ncols()) {
    coords c{pos.row, pos.col + 1};
    if (map.at(c.row, c.col) != '#') {
      *enqueuer = c;
    }
  }
}

auto fmt_map(xmas::views::text_matrix map, xmas::matrix<std::size_t>& distance) {
  return xmas::lazy_string([map, &distance]() -> std::string {
    std::stringstream ss;
    for (std::size_t i = 0; i < map.nrows(); ++i) {
      for (std::size_t j = 0; j < map.ncols(); ++j) {
        ss << [&]() {
          if (distance[i][j] == sentinel)
            return map.at(i, j);
          if (distance[i][j] % 2 == 0)
            return 'E';
          return 'O';
        }();
      }
      ss << '\n';
    }
    return ss.str();
  });
}

std::pair<std::size_t, std::size_t> count_parities(
  xmas::views::text_matrix map, std::size_t nsteps, coords start) {
  xmas::matrix<std::size_t> distance(map.nrows(), map.ncols(), sentinel);

  std::vector queue{start};
  for (std::size_t s = 0; s <= nsteps; ++s) {
    auto batch = std::move(queue);
    xlog::debug("Loaded a batch of {} entries", batch.size());

    queue = {};
    for (auto pos : batch) {
      bfs(map, distance, s, pos, std::back_inserter(queue));
    }

    if (queue.empty()) {
      break;
    }
  }

  xlog::debug("Map:\n{}", fmt_map(map, distance));

  return std::transform_reduce(
    std::execution::par_unseq, distance.cdata().begin(), distance.cdata().end(),
    std::pair<std::size_t, std::size_t>{0, 0},
    [](auto l, auto r) -> std::pair<std::size_t, std::size_t> {
      return {l.first + r.first, l.second + r.second};
    },
    [](std::size_t d) -> std::pair<std::size_t, std::size_t> {
      if (d == sentinel) {
        return {0, 0};
      }
      if (d % 2 == 0) {
        return {1, 0};
      }
      return {0, 1};
    });
}

}

std::uint64_t Day21::part1_generalized(std::size_t nsteps) {
  xmas::views::text_matrix map(this->input);
  auto start = find_start(map);
  xlog::debug("Start position is ({}, {})", start.row, start.col);

  auto [even, odd] = count_parities(map, nsteps, start);

  xlog::debug("Parity counts: {} even and {} odd", even, odd);

  if (nsteps % 2 == 0) {
    return even;
  }
  return odd;
}

std::uint64_t Day21::part1() {
  return part1_generalized(64);
}

std::uint64_t Day21::part2() {
  throw std::runtime_error("Not implemented");
}
