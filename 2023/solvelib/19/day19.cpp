#include "day19.hpp"

#include "xmaslib/line_iterator/line_iterator.hpp"
#include "xmaslib/parsing/parsing.hpp"
#include "xmaslib/lazy_string/lazy_string.hpp"
#include "xmaslib/log/log.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <execution>
#include <format>
#include <limits>
#include <map>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace {

using amount_t = unsigned;

using item = std::array<amount_t, 4>;

enum type {
  X = 0,
  M = 1,
  A = 2,
  S = 3
};

type parse_type(char ch) {
  switch (ch) {
  case 'x':
    return X;
  case 'm':
    return M;
  case 'a':
    return A;
  case 's':
    return S;
  }
  throw std::runtime_error(std::format("Unrecognized item type {}", ch));
}

auto item_to_lazy_string(item const& item) {
  return xmas::lazy_string(
    [&] { return std::format("{{x={:>4},m={:>4},a={:>4},s={:>4}}}", item[X], item[M], item[A], item[S]); });
}

enum comparisson {
  less_than = '<',
  greater_than = '>'
};

struct rule {
  type check;
  comparisson op;
  uint value;

  std::size_t output;

  static constexpr std::size_t accepted = std::numeric_limits<std::size_t>::max();
  static constexpr std::size_t rejected = accepted - 1;

  auto lazy_string() const {
    return xmas::lazy_string([this] {
      std::stringstream ss;
      ss << [this] {
        switch (check) {
        case X:
          return 'x';
        case M:
          return 'm';
        case A:
          return 'a';
        case S:
          return 's';
        }
        return '?';
      }();
      ss << char(check) << char(op) << value << ':';
      if (output == accepted) {
        ss << 'A';
      } else if (output == rejected) {
        ss << 'R';
      } else {
        ss << output;
      }

      return ss.str();
    });
  }
};

struct workflow {
  std::vector<rule> rules;
  std::size_t fallback;

  auto lazy_string() {
    return xmas::lazy_string([this] {
      std::stringstream ss;
      ss << '{';
      for (auto const& rule : rules) {
        ss << std::format("{},", rule.lazy_string());
      }

      if (fallback == rule::accepted) {
        ss << 'A';
      } else if (fallback == rule::rejected) {
        ss << 'R';
      } else {
        ss << fallback;
      }
      ss << '}';

      return ss.str();
    });
  }
};

void parse_workflow(std::string_view input, std::vector<workflow>& workflows,
  std::map<std::string_view, std::size_t>& workflow_name_to_id) {

  auto find_or_allocate_id = [&](std::string_view workflow_name) -> std::size_t {
    auto [out, inserted] = workflow_name_to_id.emplace(workflow_name, workflows.size());
    if (inserted) {
      workflows.push_back(workflow{});
    }
    return out->second;
  };

  auto it = std::ranges::find(input, '{');
  if (it == input.end()) {
    throw std::runtime_error(std::format("Line {} has no '{{'", input));
  }

  auto name = std::string_view{input.begin(), it};
  std::size_t workflow_id = find_or_allocate_id(name);

  ++it;
  while (it != input.end() && *it != '}') {
    constexpr std::string_view name_delim = "}><";
    auto out = std::find_first_of(it, input.end(), name_delim.begin(), name_delim.end());
    assert(out != input.end());
    if (*out == '}') {
      // We're at the last entry: the fallback
      std::string_view dest(it, out);
      workflows[workflow_id].fallback = find_or_allocate_id(dest);
      break;
    }

    // We're at a rule
    rule r;
    r.check = parse_type(*it); // x,m,a,s
    ++it;

    r.op = comparisson(*it);
    ++it;

    auto p = xmas::parse_int_until_break<amount_t>(it, input.end());
    it = p.first;
    r.value = p.second;

    assert(it != input.end() && *it == ':');
    ++it;

    auto begin = it;
    constexpr std::string_view delim = ",}";
    it = std::find_first_of(it, input.end(), delim.begin(), delim.end());

    r.output = find_or_allocate_id(std::string_view(begin, it));

    ++it; // It now points to the start of next rule (or to '}')
    workflows[workflow_id].rules.push_back(r);
  }

  xlog::debug("Parsed workflow {}:{}", workflow_id, workflows[workflow_id].lazy_string());
}

std::tuple<xmas::views::linewise::iterator, std::vector<workflow>, std::size_t> parse_workflows(
  xmas::views::linewise::iterator begin, xmas::views::linewise::iterator end) {

  std::map<std::string_view, std::size_t> workflow_name_to_id{
    {"A", rule::accepted},
    {"R", rule::rejected},
  };

  std::vector<workflow> workflows;

  auto it = begin;
  for (; it != end; ++it) {
    auto line = *it;
    if (line.empty()) {
      // End of rules
      break;
    }
    parse_workflow(line, workflows, workflow_name_to_id);
  }

  std::size_t worflow_in_position = workflow_name_to_id.at("in");
  return {it, std::move(workflows), worflow_in_position};
}

std::vector<item> parse_items(
  xmas::views::linewise::iterator begin, xmas::views::linewise::iterator end) {
  std::vector<item> items;

  for (auto it = begin; it != end; ++it) {
    auto line = *it;
    auto jt = line.begin();
    assert(*jt == '{');
    ++jt;
    item item{};
    while (jt != line.end() && *jt != '}') {
      auto target = parse_type(*jt);
      ++jt;
      assert(*jt == '=');
      ++jt;
      const auto [next, x] = xmas::parse_int_until_break<amount_t>(jt, line.end());
      item[target] = x;
      jt = next;

      ++jt; // jt now points to the start of the next entry (or to '}')
    }
    items.push_back(item);
  }

  return items;
}

std::size_t apply_workflow(workflow const& w, item const& it) {
  for (rule const& r : w.rules) {
    if (r.op == greater_than && it[r.check] > r.value) {
      return r.output;
    }
    if (r.op == less_than && it[r.check] < r.value) {
      return r.output;
    }
  }
  return w.fallback;
}

}

std::uint64_t Day19::part1() {
  xmas::views::linewise lines(input);

  auto [it, workflows, worflow0] = parse_workflows(lines.begin(), lines.end());
  ++it; // Skip empty line
  auto items = parse_items(it, lines.end());

  xlog::debug("Read {} workflows and {} items", workflows.size(), items.size());

  return std::transform_reduce(std::execution::seq, items.begin(), items.end(), std::uint64_t{0},
    std::plus<std::uint64_t>{}, [&](item const& item) -> std::uint64_t {
      std::size_t wflow_id = worflow0;
      while (wflow_id != rule::accepted && wflow_id != rule::rejected) {
        wflow_id = apply_workflow(workflows[wflow_id], item);
      }

      if (wflow_id == rule::accepted) {
        auto x = std::reduce(std::execution::unseq, item.begin(), item.end(), amount_t{0});
        xlog::debug("Item {} accepted with {} points", item_to_lazy_string(item), x);
        return x;
      }

      xlog::debug("Item {} rejected", item_to_lazy_string(item));
      return 0;
    });
}

std::uint64_t Day19::part2() {
  throw std::runtime_error("Not implemented");
}