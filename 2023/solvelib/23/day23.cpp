#include "day23.hpp"

#include "xmaslib/log/log.hpp"
#include "xmaslib/matrix/dense_matrix.hpp"
#include "xmaslib/matrix/text_matrix.hpp"

#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstddef>
#include <format>
#include <iterator>
#include <map>
#include <optional>
#include <ranges>
#include <stdexcept>
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
    node_id to;
    length_t len;
  };

  struct node {
    node_id id;
    coords position;
    std::map<node_id, length_t> neigbours;

    void add_edge(node_id to, length_t len) {
      auto it = neigbours.find(to);
      if (it != neigbours.end()) {
        it->second = std::max(it->second, len);
        return;
      }
      xlog::debug("Added edge from nodes {} to {} (len={})", id, to, len);
      neigbours.emplace_hint(it, to, len);
    }
  };

  node_id add_node(coords pos) {
    const std::size_t id = nodes.size();
    nodes.emplace_back(id, pos);
    xlog::debug("Added node {} at {}", id, pos);
    return id;
  }

  void add_edge(node_id from, node_id to, length_t len, bool bidirectional) {
    nodes[from].add_edge(to, len);

    if (!bidirectional) {
      return;
    }

    std::swap(from, to);
    add_edge(from, to, len, false);
  }

  std::optional<node_id> node_at(coords pos) const {
    auto it = std::ranges::find_if(
      nodes.begin(), nodes.end(), [&](node const& node) { return node.position == pos; });
    if (it == nodes.end()) {
      return {};
    }
    return {static_cast<node_id>(it - nodes.begin())};
  }

  std::vector<node> nodes;
};

enum heading {
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

  void step(heading towards) {
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
  xmas::dense_matrix<BOOL>& visited, std::array<temp_edge, 4>::iterator it, temp_edge tip,
  heading towards) {
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
  xmas::dense_matrix<BOOL> visited(map.nrows(), map.ncols(), FALSE);

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

struct visit_record {
private:
  constexpr static std::size_t N = 64;
  std::bitset<N> data = {};

public:
  consteval static std::size_t size() noexcept {
    return N;
  }

  bool get(graph::node_id id) const noexcept {
    return data[id];
  }

  void set(graph::node_id id) noexcept {
    data[id] = true;
  }
};

// longest_path returns the longest path from node pos to node target, if there is one at all.
std::optional<length_t> longest_path(
  graph const& g, graph::node_id pos, graph::node_id target, visit_record visited = {}) {
  if (pos == target) {
    return {0};
  }

  visited.set(pos);

  std::optional<length_t> best = {}; // The best path so far, if any

  for (auto const& [neigh, distance] : g.nodes[pos].neigbours) {
    if (visited.get(neigh)) {
      continue;
    }

    auto len = longest_path(g, neigh, target, visited);
    if (!len.has_value()) {
      visited.set(neigh); // Prune node that is not connected to the end
      continue;
    }

    best = {std::max(best.value_or(0), distance + *len)};
  }

  return best;
}

}

std::uint64_t Day23::part1() {
  xmas::views::text_matrix map(this->input);
  auto g = build_graph(map);

  if (g.nodes.size() > visit_record::size()) {
    throw std::runtime_error(
      std::format("This solution supports at most {} nodes", visit_record::size()));
  }

  auto opt = longest_path(g, 0, 1);
  if (!opt.has_value()) {
    throw std::runtime_error("Could not find a path to the exit");
  }

  return *opt;
}

std::uint64_t Day23::part2() {
  xmas::views::text_matrix map(this->input);
  auto g = build_graph(map);

  if (g.nodes.size() > visit_record::size()) {
    throw std::runtime_error(
      std::format("This solution supports at most {} nodes", visit_record::size()));
  }

  // Duplicate all edges in reverse
  for (auto const& n : g.nodes) {
    for (auto [dest, len] : n.neigbours) {
      g.add_edge(dest, n.id, len, false);
    }
  }

  auto opt = longest_path(g, 0, 1);
  if (!opt.has_value()) {
    throw std::runtime_error("Could not find a path to the exit");
  }

  return *opt;
}