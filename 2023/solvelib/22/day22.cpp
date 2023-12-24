#include "day22.hpp"

#include "xmaslib/matrix/matrix.hpp"
#include "xmaslib/line_iterator/line_iterator.hpp"
#include "xmaslib/parsing/parsing.hpp"
#include "xmaslib/lazy_string/lazy_string.hpp"
#include "xmaslib/log/log.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <format>
#include <iterator>
#include <limits>
#include <ranges>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace {

namespace rng = std::ranges;
namespace v = std::ranges::views;

using lenght_t = std::size_t;
using block_t = std::size_t;

constexpr block_t placeholder_id = std::numeric_limits<block_t>::max();

struct coords {
  lenght_t x, y, z;
};

struct block {
  coords lower;
  coords upper;

  std::size_t id;
  std::vector<block_t> supported_by{};

  void set_new_floor(lenght_t floor) {
    assert(floor < lower.z);
    const auto thickness = upper.z - lower.z;
    lower.z = floor + 1;
    upper.z = lower.z + thickness;
  }

  lenght_t size_XY() const noexcept {
    return (upper.x - lower.x + 1) * (upper.y - lower.y + 1);
  }

  auto view_XY() const {
    const std::size_t xwidth = (1 + upper.x - lower.x);
    return v::iota(std::size_t{0}, size_XY()) |
           v::transform([this, xwidth](std::size_t idx) -> coords {
             lenght_t dx = idx % xwidth;
             lenght_t dy = idx / xwidth;
             return coords{
               .x = lower.x + dx,
               .y = lower.y + dy,
               .z = 0,
             };
           });
  }
};

block parse_block(std::string_view line) {
  auto ints = xmas::parse_ints<lenght_t>(line);
  if (ints.size() != 6) {
    throw std::runtime_error(std::format("could not parse line: {}", line));
  }

  block b{
    .lower = {.x = std::min(ints[0], ints[3]),
              .y = std::min(ints[1], ints[4]),
              .z = std::min(ints[2], ints[5])},
    .upper = {.x = std::max(ints[0], ints[3]),
              .y = std::max(ints[1], ints[4]),
              .z = std::max(ints[2], ints[5])},
    .id = placeholder_id,
  };

  xlog::debug(
    "{},{},{}~{},{},{}", b.lower.x, b.lower.y, b.lower.z, b.upper.x, b.upper.y, b.upper.z);
  return b;
}

// Returns a list of blocks sorted by height (bottom to top) and the max X and Y coordinates
std::tuple<std::vector<block>, std::size_t, std::size_t> parse(std::string_view input) {
  xmas::views::linewise lines(input);
  std::vector<block> blocks;
  std::transform(lines.begin(), lines.end(), std::back_inserter(blocks), parse_block);

  rng::sort(blocks, [](block const& l, block const& r) { return l.lower.z < r.lower.z; });

  // Assign IDs
  for (std::size_t i = 0; i < blocks.size(); ++i) {
    blocks[i].id = i;
  }

  auto max_x = rng::max(blocks | v::transform([](block const& b) { return b.upper.x; }));
  auto max_y = rng::max(blocks | v::transform([](block const& b) { return b.upper.y; }));

  xlog::debug("Detected {} rows and {} columns", max_x + 1, max_y + 1);
  return {blocks, max_x, max_y};
}

[[maybe_unused]] auto fmt_map(xmas::matrix<std::size_t>& map, std::span<const block>) {
  return xmas::lazy_string([&map] {
    std::stringstream ss;
    for (auto r : map.rows()) {
      for (auto c : r) {
        if (c == placeholder_id) {
          ss << " xxxx";
          continue;
        }
        ss << std::format(" {:>4}", c);
      }
      ss << '\n';
    }
    return std::move(ss).str();
  });
}

}

std::uint64_t Day22::part1() {
  auto [blocks, max_x, max_y] = parse(input);
  xmas::matrix map(max_x + 1, max_y + 1, placeholder_id);

  std::set<block_t> cannot_remove{};

  for (block& b : blocks) {
    // clang-format off
    const auto max_z = std::ranges::max(b.view_XY() 
      | v::transform([&](coords pos) { return map[pos.x][pos.y]; }) 
      | v::transform([&](block_t id) { return id == placeholder_id ? 0 : blocks[id].upper.z; }));
    // clang-format on

    b.set_new_floor(max_z);

    // clang-format off
    auto supports = b.view_XY()
      | v::transform([&](coords pos) { return map[pos.x][pos.y]; }) 
      | v::filter([&](block_t id) { return id != placeholder_id ; })
      | v::filter([&](block_t id) { return blocks[id].upper.z == max_z ; });
    // clang-format on

    if (std::set s(supports.begin(), supports.end()); s.size() == 1) {
      cannot_remove.insert(*s.begin());
    }

    rng::for_each(b.view_XY(), [&](coords pos) { map[pos.x][pos.y] = b.id; });
  }

  return blocks.size() - cannot_remove.size();
}

std::uint64_t Day22::part2() {
  throw std::runtime_error("Not implemented");
}