#include "day08.hpp"

#include <algorithm>
#include <iterator>
#include <ranges>
#include <stdexcept>
#include <string_view>
#include <map>

namespace {

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

}

std::uint64_t Day08::part1() {
  auto [instr, network] = parse(this->input);

  auto curr_node = network.find("AAA");
  if (curr_node == network.end()) {
    throw std::runtime_error("Start node does not exist");
  }

  std::uint64_t count = 0;
  while (curr_node->first != "ZZZ") {
    auto search = [&]() -> std::string const& {
      auto move = instr.at(count % instr.size());
      if (move == 'R') {
        return curr_node->second.right;
      }
      return curr_node->second.left;
    }();
    ++count;

    curr_node = network.find(search);
    if (curr_node == network.end()) {
      throw std::runtime_error(std::format("Node '{}' does not exist", search));
    }
  }

  return count;
}

std::uint64_t Day08::part2() {
  throw std::runtime_error("Not implemented");
}
