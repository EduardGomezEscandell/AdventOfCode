#include <cassert>
#include <cstddef>
#include <execution>
#include <functional>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "day16.hpp"
#include "xmaslib/functional/functional.hpp"
#include "xmaslib/iota/iota.hpp"
#include "xmaslib/lazy_string/lazy_string.hpp"
#include "xmaslib/log/log.hpp"
#include "xmaslib/matrix/text_matrix.hpp"

namespace {

enum class direction : std::uint8_t {
  none = 0,
  left = 1,
  right = 2,
  up = 4,
  down = 8
};

enum class orientation {
  none,
  vertical,
  horizontal
};

// The beam orientation a splitter interacts with
static enum orientation splitter_orientation(char ch) {
  switch (ch) {
  case '|':
    return orientation::horizontal;
  case '-':
    return orientation::vertical;
  default:
    return orientation::none;
  }
}

struct cell {
  direction visited = direction::none;

  bool history_contains(direction x) {
    return (int(visited) & int(x)) != 0;
  }
  void set_history(direction x) {
    visited = direction(int(visited) | int(x));
  }
};

struct beam {
  direction towards;
  std::ptrdiff_t row;
  std::ptrdiff_t col;

  bool operator==(beam const& other) const {
    return row == other.row && col == other.col && towards == other.towards;
  }

  std::size_t srow() const {
    assert(row >= 0);
    return static_cast<std::size_t>(row);
  }
  std::size_t scol() const {
    assert(col >= 0);
    return static_cast<std::size_t>(col);
  }

  void step() {
    switch (towards) {
    case direction::left:
      --col;
      break;
    case direction::right:
      ++col;
      break;
    case direction::up:
      --row;
      break;
    case direction::down:
      ++row;
      break;
    case direction::none:
      throw std::runtime_error("Beam heading nowhere");
    }
  }

  orientation beam_orientation() {
    switch (towards) {
    case direction::left:
    case direction::right:
      return orientation::horizontal;

    case direction::up:
    case direction::down:
      return orientation::vertical;

    default:
      throw std::runtime_error("Beam heading nowhere");
    }
  }

  std::pair<beam, beam> split() {
    switch (beam_orientation()) {
    case orientation::vertical:
      return {
        {.towards = direction::left,  .row = row, .col = col},
        {.towards = direction::right, .row = row, .col = col}
      };
    case orientation::horizontal:
      return {
        {.towards = direction::up,   .row = row, .col = col},
        {.towards = direction::down, .row = row, .col = col}
      };
    default:
      assert(false);
      return {};
    }
  }

  bool off_limits(xmas::views::text_matrix const& map) {
    if (row < 0 || col < 0) {
      return true;
    }
    if (srow() >= map.nrows() || scol() >= map.ncols()) {
      return true;
    }

    return false;
  }

  bool beam_must_split(char pos) {
    auto o = splitter_orientation(pos);
    if (o == orientation::none) {
      return false; // No splitters
    }

    if (o == beam_orientation()) {
      return true; // Splitter interacts
    }

    return false; // Splitter is parallel
  }

  direction reflection(char pos) {
    switch (pos) {
    case '/':
      switch (towards) {
      case direction::up:
        return direction::right;
      case direction::right:
        return direction::up;
      case direction::down:
        return direction::left;
      case direction::left:
        return direction::down;
      default:
        assert(false);
      }
    case '\\':
      switch (towards) {
      case direction::up:
        return direction::left;
      case direction::left:
        return direction::up;
      case direction::down:
        return direction::right;
      case direction::right:
        return direction::down;
      default:
        assert(false);
      }
    default:
      break;
    }

    return direction::none;
  }

  std::string print_state(xmas::views::text_matrix const& map,
    std::vector<std::vector<cell>>& visited) const {
    std::stringstream ss;
    for (std::size_t i = 0; i < map.nrows(); ++i) {
      for (std::size_t j = 0; j < map.ncols(); ++j) {
        if (i == srow() && j == scol()) {
          ss << "X";
          continue;
        }
        if (auto x = map.at(i, j); x != '.') {
          ss << x;
          continue;
        }
        ss << int(visited[i][j].visited);
      }
      ss << '\n';
    }
    return ss.str();
  }

  std::vector<beam> advance(xmas::views::text_matrix const& map,
    std::vector<std::vector<cell>>& visited) {
    std::vector<beam> new_beams;
    while (true) {
      step();
      // xlog::debug("\nSTEP\n{}", print_state(map, visited));

      if (off_limits(map)) {
        break;
      }

      auto& cell = visited[srow()][scol()];
      if (cell.history_contains(towards)) {
        break;
      }
      cell.set_history(towards);

      char pos = map.at(srow(), scol());

      if (auto d = reflection(pos); d != direction::none) {
        towards = d;
        continue;
      }

      if (!beam_must_split(pos)) {
        continue;
      }

      // We follow one of the beams and push back the other one
      auto [beam1, beam2] = split();
      *this = beam1;
      new_beams.push_back(beam2);
    }

    return new_beams;
  }
};
} // namespace

namespace {

std::uint64_t solve(xmas::views::text_matrix const& map, beam init_beam) {
  std::vector visited(map.nrows(), std::vector<cell>(map.ncols(), cell{}));
  std::vector<beam> beams{init_beam};

  while (!beams.empty()) {
    auto curr = std::move(beams);
    beams = {};
    beams.reserve(2 * curr.size());

    for (auto b : curr) {
      auto new_beams = b.advance(map, visited);
      for (auto& nb : new_beams) {
        beams.push_back(nb);
      }
    }
  }

  auto x = std::transform_reduce(std::execution::unseq, visited.begin(), visited.end(),
    std::uint64_t{0}, std::plus<std::uint64_t>{}, [](auto const& row) {
      return std::transform_reduce(std::execution::unseq, row.begin(), row.end(), std::uint64_t{0},
        std::plus<std::uint64_t>{}, [](cell c) { return int(c.visited) != 0u; });
    });

  xlog::debug("Running {} from ({:>3},{:>3}) results in {:>3} visited cells",
    xmas::lazy_string([&]() -> std::string {
      switch (init_beam.towards) {
      case direction::right:
        return "LEFT ";
      case direction::left:
        return "RIGHT";
      case direction::up:
        return "UP   ";
      case direction::down:
        return "DOWN ";
      default:
        return std::format("{:>4}?", int(init_beam.towards));
      }
    }),
    init_beam.row, init_beam.col, x);

  return x;
}

} // namespace

std::uint64_t Day16::part1() {
  xmas::views::text_matrix map(this->input);
  return solve(map, {.towards = direction::right, .row = 0, .col = -1});
}

std::uint64_t Day16::part2() {
  xmas::views::text_matrix map(this->input);

  // Iterate over all rows, entering from left and right every iteration
  xmas::views::iota<std::size_t> rows(map.nrows());
  auto h = std::transform_reduce(std::execution::par_unseq, rows.begin(), rows.end(),
    std::uint64_t{0}, xmas::max<std::uint64_t>{}, [&map](std::size_t row) {
      auto from_left =
        solve(map, {.towards = direction::right, .row = std::ptrdiff_t(row), .col = -1});
      auto from_right = solve(map, {.towards = direction::left,
                                     .row = std::ptrdiff_t(row),
                                     .col = std::ptrdiff_t(map.ncols())});
      return std::max(from_left, from_right);
    });

  // Iterate over all columns, entering from above and below every iteration
  xmas::views::iota<std::size_t> cols(map.ncols());
  auto v = std::transform_reduce(std::execution::par_unseq, cols.begin(), cols.end(),
    std::uint64_t{0}, xmas::max<std::uint64_t>{}, [&map](std::size_t col) {
      auto from_above =
        solve(map, {.towards = direction::down, .row = -1, .col = std::ptrdiff_t(col)});
      auto from_below = solve(map,
        {.towards = direction::up, .row = std::ptrdiff_t(map.nrows()), .col = std::ptrdiff_t(col)});
      return std::max(from_above, from_below);
    });
  return std::max(h, v);
}