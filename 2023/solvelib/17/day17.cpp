#include "day17.hpp"

#include "xmaslib/lazy_string/lazy_string.hpp"
#include "xmaslib/line_iterator/line_iterator.hpp"
#include "xmaslib/log/log.hpp"
#include "xmaslib/matrix/dense_matrix.hpp"
#include "xmaslib/matrix/text_matrix.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <limits>
#include <queue>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {

// Ordered clock-wise
enum heading {
  N = 0,
  E = 1,
  S = 2,
  W = 3
};

enum relative_heading {
  right,
  left,
  ahead,
};

using length_t = std::int64_t;
using heat_t = std::uint64_t;

struct state {
  length_t row, col;
  heading head;
  heat_t heat_lost;

  std::pair<std::size_t, std::size_t> coords() const noexcept {
    assert(row >= 0);
    assert(col >= 0);
    return {std::size_t(row), std::size_t(col)};
  }

  void update_heading(relative_heading steer) {
    switch (steer) {
    case ahead:
      return;
    case left:
      head = heading((head - 1) % 4u);
      return;
    case right:
      head = heading((head + 1) % 4u);
      return;
    }
  }

  bool step_forward(xmas::views::text_matrix map) {
    switch (head) {
    case N:
      if (row == 0) {
        return false;
      }
      --row;
      return true;
    case S:
      if (row + 1 == length_t(map.nrows())) {
        return false;
      }
      ++row;
      return true;
    case W:
      if (col == 0) {
        return false;
      }
      --col;
      return true;
    case E:
      if (col + 1 == length_t(map.ncols())) {
        return false;
      }
      ++col;
      return true;
    }

    assert(false);
    return false;
  }

  bool move(xmas::views::text_matrix map, std::pair<relative_heading, length_t> move) {
    update_heading(move.first);

    for (length_t i = 0; i < move.second; ++i) {
      bool ok = step_forward(map);
      if (!ok) {
        return false;
      }
      heat_lost += heat_t(map.at(coords()) - '0');
    }

    return true;
  }
};

std::pair<std::size_t, std::size_t> bottom_right(xmas::views::text_matrix map) {
  return {map.nrows() - 1, map.ncols() - 1};
}

// Possible moves assuming that a no step has already been taken
// in the current heading.
void possible_moves(xmas::views::text_matrix map, state s, std::size_t min_steps,
  std::size_t max_steps, std::invocable<state> auto append) {

  auto exit = bottom_right(map);

  // Advance from MIN to MAX blocks
  for (std::size_t i = min_steps; i <= max_steps; ++i) {
    // Reached the end?
    if (s.coords() == exit) {
      append(s);
    }

    // Turn left?
    state l = s;
    if (bool ok = l.move(map, {left, min_steps}); ok) {
      append(l);
    }

    // Turn right?
    state r = s;
    if (bool ok = r.move(map, {right, min_steps}); ok) {
      append(r);
    }

    // Step forward
    if (bool ok = s.move(map, {ahead, 1}); !ok) {
      return;
    }
  }
}

using queue = std::priority_queue<state, std::vector<state>, bool (*)(state const&, state const&)>;

[[maybe_unused]] auto fmt_curr_state(std::array<xmas::dense_matrix<heat_t>, 4> const& visited,
  state s) {
  return xmas::lazy_string([&visited, s] {
    std::stringstream ss;
    for (std::size_t i = 0; i < visited[0].nrows(); ++i) {
      for (std::size_t j = 0; j < visited[0].nrows(); ++j) {

        if (i == std::size_t(s.row) && j == std::size_t(s.col)) {
          ss << "#";
          continue;
        }

        auto c = std::ranges::count_if(
          visited | std::ranges::views::transform([=](auto const& v) { return v[i][j]; }),
          [](heat_t h) { return h != std::numeric_limits<heat_t>::max(); });
        if (c == 0) {
          ss << ".";
        } else {
          ss << c;
        }
      }
      ss << '\n';
    }

    return std::move(ss).str();
  });
}

heat_t distance_to_end(xmas::views::text_matrix map, std::pair<std::size_t, std::size_t> pos) {
  auto end = bottom_right(map);

  return (end.first - pos.first) + (end.second - pos.second);
}

struct heap {
  std::vector<state> data;

  bool empty() {
    return data.empty();
  }

  void push(state const& s) {
    data.push_back(s);
    std::push_heap(data.begin(), data.end(), heuristic);
  }

  state pop() {
    std::pop_heap(data.begin(), data.end(), heuristic);
    state s = data.back();
    data.pop_back();
    return s;
  }

  void purge_redundant(heat_t best_score) {
    // Remove duplicates
    std::ranges::sort(data, [](state const& l, state const& r) {
      if (l.row != r.row) {
        return l.row < r.row;
      }
      if (l.col != r.col) {
        return l.col < r.col;
      }
      return l.heat_lost < r.heat_lost;
    });

    auto new_end = std::unique(data.begin(), data.end(),
      [](state const& l, state const& r) { return l.row == r.row && l.col == r.col; });
    data.erase(new_end, data.end());

    // Remove redundancies
    new_end = std::partition(
      data.begin(), data.end(), [=](state const& a) { return a.heat_lost < best_score; });
    data.erase(new_end, data.end());

    // Re-make heap
    std::make_heap(data.begin(), data.end(), heuristic);
  }

  // Return TRUE if the left argument has priority over the right.
  static bool heuristic(state const& lhs, state const& rhs) noexcept {
    // Prioritize HIGH scores and punish cells close to the origin.
    auto lscore = std::int64_t(lhs.heat_lost) - (lhs.row + lhs.col);
    auto rscore = std::int64_t(rhs.heat_lost) - (rhs.row + rhs.col);

    return lscore > rscore;
  }
};

std::uint64_t solve(std::string& input, std::size_t min_steps, std::size_t max_steps) {
  const xmas::views::text_matrix map(input);

  // Path-finding!
  //
  // We consider a graph where every node is a player's position and their heading right after
  // turning. Intermediate states where the player has taken a step already are not considered.
  //
  // The edges of the graph connect the current state to all other possible non-intermediate
  // states. These are computed on the fly by possible_moves().
  //
  // Hence, we have four nodes (aka non-intermediate states) in every position in the map: one for
  // each heading.
  //
  // With this, the problem becomes a simple path-finding problem. I used A* to solve it. My
  // heuristic is prioritizes low heat losses and closeness to the end position. I've made the
  // choice of heuristic such that I can short-circuit once I've found a solution.
  auto max_heat = std::numeric_limits<heat_t>::max();

  std::array<xmas::dense_matrix<heat_t>, 4> visited{
    xmas::dense_matrix<heat_t>(map.nrows(), map.ncols(), max_heat),
    xmas::dense_matrix<heat_t>(map.nrows(), map.ncols(), max_heat),
    xmas::dense_matrix<heat_t>(map.nrows(), map.ncols(), max_heat),
    xmas::dense_matrix<heat_t>(map.nrows(), map.ncols(), max_heat),
  };

  heap queue;

  // Starting options
  for (heading head : {E, S}) {
    state s{.row = 0, .col = 0, .head = head, .heat_lost = 0};
    visited[head][s.coords()] = s.heat_lost;
    if (bool ok = s.move(map, {ahead, min_steps}); !ok) {
      throw std::runtime_error("Input too small");
    }
    visited[head][s.coords()] = s.heat_lost;
    queue.push(s);
  }

  auto current_best = [&]() -> heat_t {
    auto v =
      visited | std::ranges::views::transform([&](auto const& v) { return v[bottom_right(map)]; });
    return std::ranges::min(v);
  };

  while (!queue.empty()) {
    state s = queue.pop();

    if (s.heat_lost > visited[s.head][s.coords()]) {
      // Better path to this state was found, ignore current path.
      continue;
    }

    if (s.heat_lost + distance_to_end(map, s.coords()) > current_best()) {
      // Short-circuit: all states in the queue have higher heat-loss than the
      // currently found solution.
      break;
    }

    possible_moves(map, s, min_steps, max_steps, [&](state const& cont) {
      heat_t& prev = visited[cont.head][cont.coords()];
      if (prev <= cont.heat_lost) {
        return;
      }
      prev = cont.heat_lost;

      if (cont.coords() == bottom_right(map)) {
        return;
      }

      queue.push(cont);
    });
  }

  return current_best();
}

}

std::uint64_t Day17::part1() {
  return solve(this->input, 1, 3);
}

std::uint64_t Day17::part2() {
  return solve(this->input, 4, 10);
}
