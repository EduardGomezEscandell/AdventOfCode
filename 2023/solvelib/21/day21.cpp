#include "day21.hpp"

#include "xmaslib/log/log.hpp"
#include "xmaslib/matrix/dense_matrix.hpp"
#include "xmaslib/matrix/text_matrix.hpp"
#include "xmaslib/lazy_string/lazy_string.hpp"

#include <algorithm>
#include <array>
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

void bfs(xmas::views::text_matrix map, xmas::basic_matrix<std::size_t>& distances, std::size_t d,
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

[[maybe_unused]] auto fmt_map(
  xmas::views::text_matrix map, xmas::basic_matrix<std::size_t>& distance) {
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
  xmas::basic_matrix<std::size_t> distance(map.nrows(), map.ncols(), sentinel);

  std::vector queue{start};
  for (std::size_t s = 0; s <= nsteps; ++s) {
    auto batch = std::move(queue);
    queue = {};
    for (auto pos : batch) {
      bfs(map, distance, s, pos, std::back_inserter(queue));
    }

    if (queue.empty()) {
      break;
    }
  }

  // Too spammy
  // xlog::debug("Map:\n{}\n", fmt_map(map, distance));

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

namespace {
enum entrypoint {
  N,
  NE,
  E,
  SE,
  S,
  SW,
  W,
  NW,
  C, // center
};

}

std::uint64_t Day21::part2_generalized(std::size_t steps) {
  /*
  Check out the README or nothing will make sense here.
  */
  xmas::views::text_matrix map(this->input);

  if (map.ncols() != map.nrows()) {
    throw std::runtime_error("This solution assumes square blocks");
  }

  const std::size_t block_size = map.ncols();
  if (block_size % 2 == 0) {
    throw std::runtime_error("This solution assumes odd block sizes");
  }

  if ((steps - block_size / 2) % block_size != 0) {
    //      half-blocks
    // <--------> <-------->
    //           S<-------------------------->steps
    // <-------block-------><------slack----->
    throw std::runtime_error("This solution assumes the slack is a multiple of the block size");
  }

  auto start = find_start(map);
  const std::size_t half_block = block_size / 2;
  if (start.row != half_block) {
    throw std::runtime_error("This solution assumes the start point is at the center");
  }
  if (start.col != half_block) {
    throw std::runtime_error("This solution assumes the start point is at the center");
  }

  std::array<coords, 9> pos;
  pos[NW] = {0, 0};
  pos[N] = {0, half_block};
  pos[NE] = {0, block_size - 1};
  pos[W] = {half_block, 0};
  pos[C] = {half_block, half_block};
  pos[E] = {half_block, block_size - 1};
  pos[SW] = {block_size - 1, 0};
  pos[S] = {block_size - 1, half_block};
  pos[SE] = {block_size - 1, block_size - 1};

  std::vector<std::pair<std::size_t, std::size_t>> purple;
  std::vector<std::pair<std::size_t, std::size_t>> orange;
  std::vector<std::pair<std::size_t, std::size_t>> yellow;
  std::vector<std::pair<std::size_t, std::size_t>> blue;

  const std::size_t orange_len = half_block - 1;
  const std::size_t yellow_len = block_size + half_block - 1;
  const std::size_t purple_len = block_size - 1;
  const std::size_t blue_len = block_size * block_size; // Arbitrary large number

  for (auto corner : {NW, NE, SE, SW}) {
    orange.push_back(count_parities(map, orange_len, pos[corner]));
    yellow.push_back(count_parities(map, yellow_len, pos[corner]));
  }
  for (auto side : {N, W, E, S}) {
    purple.push_back(count_parities(map, purple_len, pos[side]));
  }
  blue.push_back(count_parities(map, blue_len, pos[C]));

  std::size_t N = (steps - half_block) / block_size;

  // Number of blocks for even/odd parity
  std::pair<std::size_t, std::size_t> nblue = {N * N, (N - 1) * (N - 1)};
  std::pair<std::size_t, std::size_t> norange = {N, 0};
  std::pair<std::size_t, std::size_t> nyellow = {0, N - 1};
  std::pair<std::size_t, std::size_t> npurple = {1, 0};

  auto total_blue =
    std::transform_reduce(blue.begin(), blue.end(), std::uint64_t{0}, std::plus<std::uint64_t>{},
      [&](auto parities) { return parities.first * nblue.first + parities.second * nblue.second; });

  auto total_orange = std::transform_reduce(
    orange.begin(), orange.end(), std::uint64_t{0}, std::plus<std::uint64_t>{}, [&](auto parities) {
      return parities.first * norange.first + parities.second * norange.second;
    });

  auto total_yellow = std::transform_reduce(
    yellow.begin(), yellow.end(), std::uint64_t{0}, std::plus<std::uint64_t>{}, [&](auto parities) {
      return parities.first * nyellow.first + parities.second * nyellow.second;
    });

  auto total_purple = std::transform_reduce(
    purple.begin(), purple.end(), std::uint64_t{0}, std::plus<std::uint64_t>{}, [&](auto parities) {
      return parities.first * npurple.first + parities.second * npurple.second;
    });

  xlog::debug("Detected {} cells in blue blocks", total_blue);
  xlog::debug("Detected {} cells in orange blocks", total_orange);
  xlog::debug("Detected {} cells in yellow blocks", total_yellow);
  xlog::debug("Detected {} cells in purple blocks", total_purple);

  return total_blue + total_orange + total_yellow + total_purple;
}

std::uint64_t Day21::part2() {
  return part2_generalized(26501365);
}
