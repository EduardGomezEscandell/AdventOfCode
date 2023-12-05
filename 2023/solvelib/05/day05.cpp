#include "day05.hpp"

#include "xmaslib/line_iterator/line_iterator.hpp"
#include "xmaslib/log/log.hpp"

#include <algorithm>
#include <cstdint>
#include <format>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace {

// parse_ints parses a list of integers inbetween other characters
// "534<dv13213zv  5643" -> [534, 13213, 5343]
std::vector<std::uint64_t> parse_ints(std::string_view str) {
  std::vector<std::uint64_t> out;

  std::uint64_t n = 0;  // Current value of number
  bool parsing = false; // Are we parsing a number?

  for (auto ch : str) {
    if (ch >= '0' && ch <= '9') {
      parsing = true;
      n = 10 * n + std::uint64_t(ch - '0');
      continue;
    }

    if (parsing) {
      out.push_back(n);
    }

    n = 0;
    parsing = false;
  }

  if (parsing) {
    out.push_back(n);
  }

  return out;
}

// Parses the first line of the almanac into a list of ints
std::vector<std::uint64_t> parse_seeds(std::string_view first_line) {
  auto it = std::find(first_line.cbegin(), first_line.cend(), ':');
  if (it == first_line.cend()) {
    throw std::runtime_error("first line contains no ':'");
  }

  return parse_ints({it + 1, first_line.cend()});
}

// A mapping is a source->destination (len) triplet.
struct mapping {
  std::uint64_t src, dest, len;

  enum result {
    too_big = 1,
    matches = 0,
    too_small = -1,
  };

  // Checks if a given number is within [src, src+len]. If so, it maps it into
  // [dest, dest+len)
  std::pair<result, std::uint64_t> match(std::uint64_t x) const {
    if (x < src)
      return {too_small, 0};
    if (auto delta = x - src; delta < len) {
      return {matches, dest + delta};
    }
    return {too_big, 0};
  }
};

// a translation layer, such as "seed-to-soil map"
struct layer {
  // src->dest,len triplets
  std::vector<mapping> mappings;

  // Takes a source and returns the destination
  std::uint64_t translate(std::uint64_t x) const {
    std::size_t lo = 0;
    std::size_t hi = mappings.size();

    // Binary search to find which range it belongs to (if any)
    while (lo < hi) {
      std::size_t mid = (hi + lo) / 2;
      switch (auto r = mappings[mid].match(x); r.first) {
      case mapping::matches:
        return r.second;
      case mapping::too_small:
        hi = mid;
        break;
      case mapping::too_big:
        lo = mid + 1;
        break;
      }
    }

    // No range: return as-is
    return x;
  }

  // After parsing the mappings (source->dest pairs), we sort them according to
  // the source.
  void commit() {
    std::ranges::sort(mappings, [](mapping const &a, mapping const &b) {
      return a.src < b.src;
    });

    // Validate no overlap between ranges, throw in case there is
    // That would mean a bad input (or me misunderstanding the problem :P)
#ifndef NDEBUG
    auto it = std::adjacent_find(mappings.begin(), mappings.end(),
                                 [](mapping const &prev, mapping const &next) {
                                   return prev.from + prev.len > next.from;
                                 });
    if (it != mappings.end()) {
      auto first = *it;
      auto second = *(it + 1);
      throw std::runtime_error(
          std::format("Entry ({},{},{}) overlaps with ({},{},{})", first.from,
                      first.to, first.len, second.from, second.to, second.len));
    }
#endif
  }
};

// parse_layer takes an iterator pointing to the first line to parse and the
// last line in the file. It parses a translation layer (e.g: seed-to-soil map)
// and all its mappings.
//
// return.first: The layer that was parsed
// return.second: An iterator pointing to the beginning of the next section
std::pair<layer, xmas::line_iterator> parse_layer(xmas::line_iterator begin,
                                                  xmas::line_iterator end) {
  layer l;
  auto it = begin;

  // Skip title
  ++it;
  for (; it != end; ++it) {
    auto ints = parse_ints(*it);
    if (auto size = ints.size(); size == 0) {
      // End of section: skip empty line and leave
      ++it;
      break;
    } else if (size != 3) {
      // Wrong input (or more likely: found a bug!)
      throw std::runtime_error(std::format(
          "Line '{}' has the wrong number of integers ({})", *it, size));
    }

    l.mappings.emplace_back(mapping{
        .src = ints[1],
        .dest = ints[0],
        .len = ints[2],
    });
  }

  xlog::debug("{} has {} ranges", *begin, l.mappings.size());

  l.commit();
  return {l, it};
}

} // namespace

std::uint64_t Day05::part1() {
  // Parsing
  auto rng = xmas::line_range(this->input);
  auto iline = rng.begin();

  // Parse header
  auto seeds = parse_seeds(*iline);
  xlog::debug("There are {} seeds", seeds.size());

  ++iline;
  ++iline;
  if (iline == rng.end()) {
    throw std::runtime_error("file contains only seeds");
  }

  // Parse translation layers
  std::array<layer, 7> layers;
  for (auto &l : layers) {
    auto r = parse_layer(iline, rng.end());
    l = r.first;
    iline = r.second;
  }

  // Computing each seed in parallel
  return std::transform_reduce(
      std::execution::par_unseq, seeds.cbegin(), seeds.cend(),
      std::numeric_limits<std::uint64_t>::max(),
      [](std::uint64_t x, std::uint64_t y) { return x < y ? x : y; }, /* min */
      [&layers](const std::uint64_t seed) -> std::uint64_t {
        auto x = seed;
        for (auto const &layer : layers) {
          x = layer.translate(x);
        }
        xlog::debug("{}->{}", seed, x);
        return x;
      });
}

std::uint64_t Day05::part2() { throw std::runtime_error("Not implemented"); }
