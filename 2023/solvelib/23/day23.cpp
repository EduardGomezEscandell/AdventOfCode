#include "day23.hpp"
#include "xmaslib/log/log.hpp"
#include "xmaslib/matrix/matrix.hpp"
#include "xmaslib/matrix/text_matrix.hpp"
#include <algorithm>
#include <cassert>
#include <iterator>
#include <optional>
#include <ranges>
#include <sstream>
#include <utility>
#include <vector>

namespace {

using length_t = std::uint64_t;

namespace views = std::ranges::views;

struct coords {
  length_t row, col;

  bool operator==(const auto& other) const noexcept {
    return row == other.row && col == other.col;
  }
};

}

template <>
struct std::formatter<coords> : std::formatter<std::string> {
  auto format(coords const& pos, format_context& ctx) const {
    auto f = std::format("({}, {})", pos.row, pos.col);
    return formatter<std::string>::format(f, ctx);
  }
};

namespace {

struct graph {
  using node_id = std::size_t;

  struct edge {
    node_id from;
    node_id to;
    length_t len;
  };

  struct node {
    coords position;
    std::vector<edge> neigbours;
  };

  node_id add_node(coords pos) {
    xlog::debug("Added node {} at {}", nodes.size(), pos);
    nodes.push_back({
      .position = pos,
      .neigbours = {},
    });
    return nodes.size() - 1;
  }

  void add_edge(node_id from, node_id to, length_t len, bool bidirectional) {
    xlog::debug("Added edge from nodes {} to {} (len={})", from, to, len);

    nodes[from].neigbours.emplace_back(from, to, len);

    if (!bidirectional) {
      return;
    }

    std::swap(from, to);
    add_edge(from, to, len, false);
  }

  std::optional<node_id> node_at(coords pos) {
    auto it = std::ranges::find_if(
      nodes.begin(), nodes.end(), [&](node const& node) { return node.position == pos; });
    if (it == nodes.end()) {
      return {};
    }
    return {static_cast<node_id>(it - nodes.begin())};
  }

  std::vector<node> nodes;
};

enum direction {
  N = '^',
  E = '>',
  W = '<',
  S = 'v',
};

struct temp_edge {
  graph::node_id from;
  coords pos;
  length_t len = 0;
  bool bidirectional = true;

  void step(direction towards) {
    switch (towards) {
    case N:
      --pos.row;
      ++len;
      return;
    case S:
      ++pos.row;
      ++len;
      return;
    case E:
      ++pos.col;
      ++len;
      return;
    case W:
      --pos.col;
      ++len;
      return;
    }
  }
};

// std::vector<bool> avoidance
enum BOOL : std::uint8_t {
  FALSE = BOOL(0),
  TRUE = BOOL(1),
};

std::array<temp_edge, 4>::iterator explore(xmas::views::text_matrix map,
  xmas::matrix<BOOL>& visited, std::array<temp_edge, 4>::iterator it, temp_edge tip,
  direction towards) {
  tip.step(towards);
  while (map.at(tip.pos) == char(towards)) {
    tip.bidirectional = false;
    tip.step(towards);
  }

  if (visited[tip.pos] == TRUE) {
    return it;
  }

  if (map.at(tip.pos) == '.') {
    *it = tip;
    return std::next(it);
  }

  return it;
}

void find_all_nodes(xmas::views::text_matrix map, graph& g) {
  // Intermediate nodes
  auto rows = views::iota(std::size_t{1}, map.nrows() - 1);
  std::ranges::for_each(rows, [&](std::size_t r) {
    for (std::size_t c = 1; c + 1 < map.ncols(); ++c) {
      if (map.at(r, c) == '#') {
        continue;
      }

      uint count = 0;
      count += map.at(r - 1, c) == '#' ? 0 : 1;
      count += map.at(r + 1, c) == '#' ? 0 : 1;
      count += map.at(r, c - 1) == '#' ? 0 : 1;
      count += map.at(r, c + 1) == '#' ? 0 : 1;

      if (count < 3) {
        continue;
      }

      g.add_node({r, c});
    }
  });
}

graph build_graph(xmas::views::text_matrix map) {
  graph g;
  xmas::matrix<BOOL> visited(map.nrows(), map.ncols(), FALSE);

  // Start point
  assert(map.at(0, 1) == '.');
  auto entry = g.add_node({0, 1});
  visited[0][1] = TRUE;

  // End point
  assert(map.at(map.nrows() - 1, map.ncols() - 2) == '.');
  auto exit = g.add_node({map.nrows() - 1, map.ncols() - 2});

  find_all_nodes(map, g);

  std::vector<temp_edge> tips{
    {.from = entry, .pos = coords{1, 1}, .len = 1},
  };

  while (!tips.empty()) {
    auto tip = tips.back();
    tips.pop_back();

    while (true) {
      // Are we at a node?
      if (auto opt = g.node_at(tip.pos); opt.has_value()) {
        if (tip.from == *opt) {
          // Closed loop
          break;
        }

        g.add_edge(tip.from, *opt, tip.len, tip.bidirectional);
        if (*opt == exit) {
          break;
        }
        tip.from = *opt;
        tip.bidirectional = true;
        tip.len = 0;
      } else {
        visited[tip.pos] = TRUE;
      }

      std::array<temp_edge, 3> neighbours;
      auto it = neighbours.begin();
      it = explore(map, visited, it, tip, N);
      it = explore(map, visited, it, tip, E);
      it = explore(map, visited, it, tip, S);
      it = explore(map, visited, it, tip, W);

      std::span continuations(neighbours.begin(), it);
      if (continuations.size() == 0) {
        // No continuations
        break;
      }

      tip = continuations.front();
      std::ranges::copy(continuations | views::drop(1), std::back_inserter(tips));
    }
  }

  return g;
}

length_t dfs_find_longest_path(
  graph const& g, std::vector<bool> const& visited, graph::node_id from, graph::node_id to) {
  if (from == to) {
    return 0;
  }

  std::uint64_t best = 0;
  for (auto const& edge : g.nodes.at(from).neigbours) {
    if (visited[edge.to]) {
      continue;
    }
    auto v = visited;
    v.at(edge.to) = true;
    const auto len = edge.len + dfs_find_longest_path(g, visited, edge.to, to);
    best = std::max(best, len);
  }

  return best;
}
}

std::uint64_t Day23::part1() {
  xmas::views::text_matrix map(this->input);
  auto g = build_graph(map);

  std::vector<bool> visited(g.nodes.size(), false);
  return dfs_find_longest_path(g, visited, 0, 1);
}

std::uint64_t Day23::part2() {
  throw std::runtime_error("Not implemented");
}