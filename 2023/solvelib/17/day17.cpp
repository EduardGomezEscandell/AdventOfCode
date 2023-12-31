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
#include <unordered_map>
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
  reverse
};

using length_t = std::int64_t;
using heat_t = std::uint64_t;

struct state {
  length_t row, col;
  heading head;
  heat_t heat_lost;

  std::pair<std::size_t, std::size_t> coords() const noexcept {
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
    case reverse:
      head = heading((head + 2) % 4u);
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

  bool move(xmas::views::text_matrix map,
    std::initializer_list<std::pair<relative_heading, length_t>>
      moves) {
    for (auto const& move_ : moves) {
      bool ok = move(map, move_);
      if (!ok) {
        return false;
      }
    }
    return true;
  }
};

std::pair<std::size_t, std::size_t> bottom_right(xmas::views::text_matrix map) {
  return {map.nrows() - 1, map.ncols() - 1};
}

// Possible moves assuming that a single step has already been taken
// in the current heading.
void possible_moves(xmas::views::text_matrix map, state s, std::invocable<state> auto append) {

  // Turn left
  {
    state n = s;
    if (bool ok = n.move(map, {left, 1}); ok) {
      append(n);
    }
  }

  // Turn right
  {
    state n = s;
    if (bool ok = n.move(map, {right, 1}); ok) {
      append(n);
    }
  }

  // clang-format off
  // Disabling clang-format because it messes with s.move(...)

  // Ahead, then left
  {
    state n = s;
    if(bool ok = n.move(map, {{ahead, 1}, {left, 1}}); ok) {
      append(n);
    }
  }

  // Ahead, then right
  {
    state n = s;
    if(bool ok = n.move(map, {{ahead, 1}, {right, 1}}); ok) {
      append(n);
    }
  }

  // Ahead 2, then left
  {
    state n = s;
    if(bool ok = n.move(map, {{ahead, 2}, {left, 1}}); ok) {
      append(n);
    }
  }

  // Ahead 2, then right
  {
    state n = s;
    if(bool ok = n.move(map, {{ahead, 2}, {right, 1}}); ok) {
      append(n);
    }
  }

  // clang-format on

  // Special case: moving ahead reaches the end
  {
    state n = s;
    bool ok = n.move(map, {ahead, 1});
    if (ok && n.coords() == bottom_right(map)) {
      append(n);
    }
  }

  {
    state n = s;
    bool ok = n.move(map, {ahead, 2});
    if (ok && n.coords() == bottom_right(map)) {
      append(n);
    }
  }
}

using queue = std::priority_queue<state, std::vector<state>, bool (*)(state const&, state const&)>;

[[maybe_unused]] auto fmt_curr_state(std::array<xmas::basic_matrix<heat_t>, 4> const& visited,
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

  void purge_duplicates() {
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

    std::make_heap(data.begin(), data.end(), heuristic);
  }

  static bool heuristic(state const& l, state const& r) noexcept {
    if (l.heat_lost != r.heat_lost) {
      return l.heat_lost > r.heat_lost;
    }

    // Prioritize more advanced positions
    if (l.row != r.row) {
      return l.row > r.row;
    }

    if (l.col != r.col) {
      return l.col > r.col;
    }

    if (l.head == r.head) {
      return true; // Arbitrary
    }

    // Prioritize moving south-east
    if (l.head == E) {
      return true;
    }
    if (r.head == E) {
      return false;
    }

    if (l.head == S) {
      return true;
    }
    if (r.head == S) {
      return false;
    }

    // Arbitrary end result
    return l.head < r.head;
  }
};
}

std::uint64_t Day17::part1() {
  xmas::views::text_matrix map(this->input);

  heap queue;

  // Path-finding!
  //
  // We only consider the state where a single step has been taken. This step can be taken
  // in four different directions, so there are 4 possible states at every position. Hence
  // we keep four cost matrices.
  auto max_heat = std::numeric_limits<heat_t>::max();

  std::array<xmas::basic_matrix<heat_t>, 4> visited{
    xmas::basic_matrix<heat_t>(map.nrows(), map.ncols(), max_heat),
    xmas::basic_matrix<heat_t>(map.nrows(), map.ncols(), max_heat),
    xmas::basic_matrix<heat_t>(map.nrows(), map.ncols(), max_heat),
    xmas::basic_matrix<heat_t>(map.nrows(), map.ncols(), max_heat),
  };

  auto current_best = [&]() -> heat_t {
    auto v = visited | std::ranges::views::transform(
                         [&](auto const& visited) { return visited[bottom_right(map)]; });
    return std::ranges::min(v);
  };

  // Case 1: start going east
  state r{.row = 0, .col = 0, .head = E, .heat_lost = 0};
  if (bool ok = r.move(map, {ahead, 1}); !ok) {
    throw std::runtime_error("The map is less than 2 columns wide");
  }
  visited[E][r.coords()] = r.heat_lost;
  queue.push(r);

  // Case 2: start going south
  state s{.row = 0, .col = 0, .head = S, .heat_lost = 0};
  if (bool ok = s.move(map, {ahead, 1}); !ok) {
    throw std::runtime_error("The map is less than 2 columns wide");
  }
  visited[S][s.coords()] = s.heat_lost;
  queue.push(s);

  for (std::size_t i = 1; !queue.empty(); ++i) {
    if (i % map.size() == 0) {
      //    ^ Every now and then, we remove duplicates
      // Not sure what the optimum frequency is, so I settled for N
      // where N is the number of nodes.
      queue.purge_duplicates();
      // xlog::debug("{}", fmt_curr_state(visited, s)); // Too verbose
    }

    state s = queue.pop();

    if (s.heat_lost > visited[s.head][s.coords()]) {
      // Better path was found, escape
      continue;
    }

    auto best = current_best();
    if (s.heat_lost > best) {
      break;
    }

    if (s.heat_lost + distance_to_end(map, s.coords()) > best) {
      continue;
    }

    possible_moves(map, s, [&](state const& s) {
      heat_t& prev = visited[s.head][s.coords()];
      if (prev < s.heat_lost) {
        return;
      }
      prev = s.heat_lost;

      if (s.coords() == bottom_right(map)) {
        // No need to continue path-finding
        return;
      }

      queue.push(s);
    });
  }

  return current_best();
}

std::uint64_t Day17::part2() {
  throw std::runtime_error("Not implemented");
}
