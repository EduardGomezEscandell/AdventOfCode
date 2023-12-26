#include "day08.hpp"
#include "xmaslib/iota/iota.hpp"
#include "xmaslib/log/log.hpp"
#include "xmaslib/matrix/matrix.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <execution>
#include <iterator>
#include <limits>
#include <linux/limits.h>
#include <numeric>
#include <ranges>
#include <set>
#include <stdexcept>
#include <string_view>
#include <map>
#include <vector>

namespace {

constexpr std::uint64_t infinity = std::numeric_limits<std::uint64_t>::max();

struct node {
  std::string left, right;
};

std::pair<std::string, node> parse_line(std::string_view line) {
  auto it = std::find(line.begin(), line.end(), ' ');

  std::pair<std::string, node> ret{
    {line.begin(), it},
    node{},
  };

  auto begin = std::find(it, line.end(), '(') + 1; // +1 to skip the coma
  auto end = std::find(begin, line.end(), ',');
  ret.second.left = {begin, end};

  begin = end + 2;                         // +2 to skip the comma and whitespace
  end = std::find(begin, line.end(), ')'); // Skip the )
  ret.second.right = {begin, end};

  return ret;
}

std::pair<std::string_view, std::map<std::string, node>> parse(std::string& input) {
  auto it = std::find(input.begin(), input.end(), '\n');

  std::string_view instructions(input.begin(), it);
  std::map<std::string, node> nodes;

  // clang-format off
  auto parsing = std::string_view{it + 2, input.end()} 
      | std::ranges::views::split('\n')
      | std::ranges::views::transform([](auto v) { return std::string_view{v.begin(), v.end()}; })
      | std::ranges::views::filter([](auto sv) { return sv.size() != 0; })
      | std::ranges::views::transform(parse_line);
  // clang-format on

  std::ranges::copy(parsing, std::inserter(nodes, nodes.begin()));

  return {instructions, nodes};
}

auto find(std::map<std::string, node> const& network, std::string const& node_name) {
  auto it = network.find(node_name);
#ifndef NDEBUG
  if (it == network.cend()) {
    throw std::runtime_error(std::format("Node '{}' does not exist", node_name));
  }
#endif
  return it;
}

std::optional<std::uint64_t> solve(std::string_view instr,
  std::map<std::string, node> const& network, std::string const& from, std::string const& to) {

  auto curr_node = find(network, from);

  std::set<std::pair<std::string const*, std::uint64_t>> visited{};

  std::uint64_t count = 0;
  while (curr_node->first != to) {
    auto cursor = count % instr.size();

    auto [it, is_new] = visited.emplace(&curr_node->first, cursor);
    if (!is_new) {
      xlog::debug("Routing {} to {} is impossible", from, to);
      return {};
    }

    auto search = [&]() -> std::string const& {
      auto move = instr.at(cursor);
      if (move == 'R') {
        return curr_node->second.right;
      }
      return curr_node->second.left;
    }();
    ++count;
    curr_node = find(network, search);
  }

  xlog::debug("Routing {} to {} takes {} cycles", from, to, count);
  return {count};
}

}

std::uint64_t Day08::part1() {
  auto [instr, network] = parse(this->input);
  auto opt = solve(instr, network, "AAA", "ZZZ");
  if (!opt.has_value()) {
    throw std::runtime_error("Loop detected");
  }
  return *opt;
}

namespace {

auto starting_nodes(std::map<std::string, node> const& network) {
  // clang-format off
  auto starters = network
    | std::ranges::views::transform([](auto pair) { return pair.first; }) 
    | std::ranges::views::filter([](std::string const& name) { return name.back() == 'A'; });
  // clang-format on
  return std::vector<std::string>(starters.begin(), starters.end());
}

auto finishing_nodes(std::map<std::string, node> const& network) {
  // clang-format off
  auto finishers = network
    | std::ranges::views::transform([](auto pair) { return pair.first; }) 
    | std::ranges::views::filter([](std::string const& name) { return name.back() == 'Z'; });
  // clang-format on
  return std::vector<std::string>(finishers.begin(), finishers.end());
}

auto timings_matrix(std::string_view instr, std::map<std::string, node> const& network,
  std::span<std::string> sources, std::span<std::string> destinations) {

  xmas::matrix<std::uint64_t> timing_matrix(sources.size(), destinations.size(), 0);

  xmas::views::iota<std::size_t> rows(0, timing_matrix.nrows());
  xmas::views::iota<std::size_t> cols(0, timing_matrix.ncols());

  std::for_each(std::execution::par_unseq, rows.begin(), rows.end(), [&](std::size_t row) {
    std::transform(cols.begin(), cols.end(), timing_matrix.row(row).begin(), [&](std::size_t col) {
      return solve(instr, network, sources[row], destinations[col]).value_or(infinity);
    });
  });

  return timing_matrix;
}

std::uint64_t compute_permutation(
  xmas::matrix<std::uint64_t> timings, std::vector<std::uint64_t> selected) {
  assert(selected.size() == timings.nrows());

  std::vector<std::uint64_t> t(selected.size());
  for (std::size_t s = 0; s < selected.size(); ++s) {
    t[s] = timings[s][selected[s]];
  }

  return std::reduce(std::execution::unseq, t.begin(), t.end(), std::uint64_t{1},
    std::lcm<std::uint64_t, std::uint64_t>);
}

std::uint64_t find_best_permutation(
  xmas::matrix<std::uint64_t> timings, std::vector<std::uint64_t>& selected_finishers) {
  std::size_t row = selected_finishers.size();
  if (row == timings.nrows()) {
    return compute_permutation(timings, selected_finishers);
  }

  std::size_t best = infinity;
  selected_finishers.push_back(infinity); // Will be overwritten

  for (std::size_t col = 0; col < timings.ncols(); ++col) {
    if (timings[row][col] == infinity) {
      continue;
    }

    if (std::ranges::find(selected_finishers, col) != selected_finishers.end()) {
      // Two starters cannot end at the same position
      continue;
    }

    selected_finishers.back() = col;
    best = std::min(best, find_best_permutation(timings, selected_finishers));
  }

  selected_finishers.pop_back();
  return best;
}

}

std::uint64_t Day08::part2() {
  auto [instr, network] = parse(this->input);

  // Find all STARTER and FINISHER nodes
  auto starters = starting_nodes(network);
  auto finishers = finishing_nodes(network);

  // Pre-compute the timings to get from every STARTER to every FINISHER
  auto matrix = timings_matrix(instr, network, starters, finishers);

  // Every starter must end at a different finisher
  // Find every valid permutation, and compute the Least Common Multiple
  // of the timings of every starter
  std::vector<std::uint64_t> preselected{};
  auto best = find_best_permutation(matrix, preselected);

  if (best == infinity) {
    throw std::runtime_error("No solution found");
  }
  return best;
}
