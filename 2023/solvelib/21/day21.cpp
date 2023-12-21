#include "day21.hpp"

#include "xmaslib/log/log.hpp"
#include "xmaslib/lru/lru.hpp"
#include "xmaslib/matrix/matrix.hpp"
#include "xmaslib/matrix/text_matrix.hpp"

#include <algorithm>
#include <cstddef>
#include <execution>
#include <functional>
#include <numeric>
#include <span>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace {

struct coords {
  std::size_t row, col;
};

struct state {
  coords c;
  std::size_t remaining_depth;

  bool operator==(state const& other) const noexcept {
    return c.row == other.c.row && c.col == other.c.col && remaining_depth == other.remaining_depth;
  }

  std::size_t hash() const noexcept {
    // Cantor pairing function from
    // https://stackoverflow.com/questions/38965931/hash-function-for-3-integers
    constexpr auto cantor = [](std::size_t a, std::size_t b) {
      return ((a + b) * (a + b + 1)) / 2 + b;
    };
    return cantor(cantor(c.row, c.col), remaining_depth);
  }
};

}

template <>
struct std::hash<state> {
  std::size_t operator()(state const& s) const noexcept {
    return s.hash();
  }
};

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

// std::vector<bool> avoidance shenaningans
using BOOL = std::uint8_t;
constexpr BOOL TRUE = 1u;
constexpr BOOL FALSE = 0u;

void bfs(xmas::lru_cache<state, int>& lru, xmas::views::text_matrix map,
  xmas::matrix<BOOL>& reachable, std::size_t remaining_depth, coords pos) {

  if (auto opt = lru.get({pos, remaining_depth}); opt.has_value()) {
    return;
  }
  lru.set({pos, remaining_depth}, 0);

  if (remaining_depth == 0) {
    reachable[pos.row][pos.col] = TRUE;
    return;
  }

  std::vector<coords> continuations;
  if (pos.row != 0) {
    coords c{pos.row - 1, pos.col};
    if (map.at(c.row, c.col) != '#') {
      continuations.push_back(c);
    }
  }
  if (pos.row + 1 < map.nrows()) {
    coords c{pos.row + 1, pos.col};
    if (map.at(c.row, c.col) != '#') {
      continuations.push_back(c);
    }
  }
  if (pos.col != 0) {
    coords c{pos.row, pos.col - 1};
    if (map.at(c.row, c.col) != '#') {
      continuations.push_back(c);
    }
  }
  if (pos.col + 1 < map.ncols()) {
    coords c{pos.row, pos.col + 1};
    if (map.at(c.row, c.col) != '#') {
      continuations.push_back(c);
    }
  }

  for (auto const& c : continuations) {
    bfs(lru, map, reachable, remaining_depth - 1, c);
  }
}
}

std::uint64_t Day21::part1_generalized(std::size_t nsteps) {
  xmas::views::text_matrix map(this->input);
  xmas::matrix<BOOL> reachable(map.nrows(), map.ncols(), FALSE);
  auto start = find_start(map);
  xlog::debug("Start position is ({}, {})", start.row, start.col);

  xmas::lru_cache<state, int> lru(100'000);
  bfs(lru, map, reachable, nsteps, start);

  return std::transform_reduce(std::execution::par_unseq, reachable.cdata().begin(),
    reachable.cdata().end(), std::uint64_t{0}, std::plus<std::uint64_t>{},
    [](BOOL reachable) -> std::uint64_t { return reachable == TRUE ? 1 : 0; });
}

std::uint64_t Day21::part1() {
  return part1_generalized(64);
}

std::uint64_t Day21::part2() {
  throw std::runtime_error("Not implemented");
}
