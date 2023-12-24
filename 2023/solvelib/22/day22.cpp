#include "day22.hpp"

#include "xmaslib/functional/functional.hpp"
#include "xmaslib/matrix/matrix.hpp"
#include "xmaslib/line_iterator/line_iterator.hpp"
#include "xmaslib/parsing/parsing.hpp"
#include "xmaslib/lazy_string/lazy_string.hpp"
#include "xmaslib/log/log.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <execution>
#include <format>
#include <functional>
#include <iterator>
#include <limits>
#include <numeric>
#include <ranges>
#include <set>
#include <span>
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

  std::set<block_t> supporting{};
  std::set<block_t> supported_by{};

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

  return {
    .lower = {.x = std::min(ints[0], ints[3]),
              .y = std::min(ints[1], ints[4]),
              .z = std::min(ints[2], ints[5])},
    .upper = {.x = std::max(ints[0], ints[3]),
              .y = std::max(ints[1], ints[4]),
              .z = std::max(ints[2], ints[5])},
    .id = placeholder_id,
  };
}

// Returns a list of blocks sorted by height (bottom to top)
std::vector<block> parse(std::string_view input) {
  xmas::views::linewise lines(input);
  std::vector<block> blocks;
  std::transform(lines.begin(), lines.end(), std::back_inserter(blocks), parse_block);

  rng::sort(blocks, [](block const& l, block const& r) { return l.lower.z < r.lower.z; });

  // Assign IDs
  for (std::size_t i = 0; i < blocks.size(); ++i) {
    blocks[i].id = i;
  }

  return blocks;
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

void simulate_drop(std::span<block> blocks) {
  auto max_x = rng::max(blocks | v::transform([](block const& b) { return b.upper.x; }));
  auto max_y = rng::max(blocks | v::transform([](block const& b) { return b.upper.y; }));

  xmas::matrix map(max_x + 1, max_y + 1, placeholder_id);
  xlog::debug("Detected {} rows and {} columns", map.nrows(), map.ncols());

  for (block& b : blocks) {
    // clang-format off
    const auto max_z = std::ranges::max(b.view_XY() 
      | v::transform([&](coords pos) { return map[pos.x][pos.y]; }) 
      | v::transform([&](block_t id) { return id == placeholder_id ? 0 : blocks[id].upper.z; }));
    // clang-format on

    b.set_new_floor(max_z);

    // Update dependencies

    // clang-format off
    auto supports = b.view_XY()
      | v::transform([&](coords pos) { return map[pos.x][pos.y]; }) 
      | v::filter([&](block_t id) { return id != placeholder_id ; })
      | v::filter([&](block_t id) { return blocks[id].upper.z == max_z ; });
    // clang-format on

    b.supported_by = std::set(supports.begin(), supports.end());
    rng::for_each(supports, [&](block_t id) { blocks[id].supporting.insert(b.id); });

    // Update map
    rng::for_each(b.view_XY(), [&](coords pos) { map[pos.x][pos.y] = b.id; });
  }
}
}

std::uint64_t Day22::part1() {
  auto blocks = parse(this->input);
  simulate_drop(blocks);

  // clang-format off
  auto critical_blocks = blocks 
    | v::filter([](block const& b) { return b.supported_by.size() == 1; })
    | v::transform([](block const& b) { return *b.supported_by.begin(); });
  // clang-format on

  std::set<block_t> cannot_remove(critical_blocks.begin(), critical_blocks.end());

  return blocks.size() - cannot_remove.size();
}

namespace {

// Assuming both ranges are sorted, return TRUE if all elements of 'contained' are inside
// 'container', and FALSE otherwise.
bool is_subset(auto const& contained, auto const& container) {
  if (contained.size() > container.size()) {
    return false;
  }

  auto it = contained.begin();
  auto jt = container.begin();

  for (; it != contained.end(); ++it) {
    jt = std::partition_point(jt, container.end(), xmas::less_than(*it));
    if (jt == container.end()) {
      return false;
    }

    if (*it == *jt) {
      continue;
    }

    return false;
  }
  return true;
}

std::size_t count_chain_reaction(std::span<const block> blocks, block_t root) {
  std::set<block_t> upstream{root};
  std::set<block_t> tip{root};

  while (!tip.empty()) {
    std::set<block_t> candidates;
    rng::for_each(tip, [&](block_t id) {
      rng::copy(blocks[id].supporting, std::inserter(candidates, candidates.end()));
    });

    // clang-format off
    auto selected = candidates 
      | v::transform([&](block_t id) { return blocks[id]; })
      | v::filter([&](block const& b) { return is_subset(b.supported_by, upstream); }) 
      | v::transform([](block const& b) { return b.id; });
    // clang-format on

    upstream.insert(tip.begin(), tip.end());
    tip = std::set(selected.begin(), selected.end());
  }

  return upstream.size() - 1;
}

}

std::uint64_t Day22::part2() {
  auto blocks = parse(this->input);
  simulate_drop(blocks);

  return std::transform_reduce(std::execution::par_unseq, blocks.begin(), blocks.end(),
    std::uint64_t{0}, std::plus<std::uint64_t>{},
    [&](block const& b) { return count_chain_reaction(blocks, b.id); });
}