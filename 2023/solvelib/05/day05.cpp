#include "day05.hpp"

#include "xmaslib/line_iterator/line_iterator.hpp"
#include "xmaslib/log/log.hpp"
#include "xmaslib/parsing/parsing.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <execution>
#include <format>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace {

// Parses the first line of the almanac into a list of ints
std::vector<std::uint64_t> parse_seeds(std::string_view first_line) {
  auto it = std::find(first_line.cbegin(), first_line.cend(), ':');
  if (it == first_line.cend()) {
    throw std::runtime_error("first line contains no ':'");
  }

  return xmas::parse_ints<std::size_t>({it + 1, first_line.cend()});
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

struct intrange {
  std::uint64_t begin, end;
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

  // Takes a set of sources and returns the destinations
  // No guarantess on output ordering
  std::vector<intrange> translate(intrange in) const {
    std::vector<intrange> out;

    // Process the input range from left to right, until we have no input range
    // left.
    for (mapping const &m : mappings) {
      if (m.src + m.len < in.begin) {
        // mapping is to the left of the input range
        continue;
      }
      if (in.end <= m.src) {
        // mapping is to the right of the input range
        continue;
      }

      // There is an overlap

      // Snip leading part of input range (stays as-is)
      if (in.begin < m.src) {
        out.push_back({.begin = in.begin, .end = m.src});
        in.begin = m.src;
      }

      // Extact the overlap (the rest of the input will be processed next
      // iteration)
      auto overlap = intrange{
          .begin = in.begin,
          .end = std::min(in.end, m.src + m.len),
      };
      in.begin = overlap.end;

      // Map the overlap from src to dest
      auto begin_delta = overlap.begin - m.src;
      auto end_delta = overlap.end - m.src;
      out.push_back({
          .begin = m.dest + begin_delta,
          .end = m.dest + end_delta,
      });

      // Process right tail (if there is one)
      if (in.begin >= in.end) {
        break;
      }
    }

    // Append the right tail
    if (in.begin < in.end) {
      out.push_back(in);
    }

    return out;
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
                                   return prev.src + prev.len > next.src;
                                 });
    if (it != mappings.end()) {
      auto first = *it;
      auto second = *(it + 1);
      throw std::runtime_error(std::format(
          "Entry ({},{},{}) overlaps with ({},{},{})", first.src, first.dest,
          first.len, second.src, second.dest, second.len));
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
std::pair<layer, xmas::views::linewise::iterator>
parse_layer(xmas::views::linewise::iterator begin,
            xmas::views::linewise::iterator end) {
  layer l;
  auto it = begin;

  // Skip title
  ++it;
  for (; it != end; ++it) {
    auto ints = xmas::parse_ints<std::size_t>(*it);
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

// coalesce_ranges takes a set of ranges and merges overlapping one.
std::vector<intrange> coalesce_ranges(std::vector<intrange> in) {
  if (in.size() < 2) {
    return in;
  }

  // Sort according to range start
  std::sort(in.begin(), in.end(),
            [](intrange a, intrange b) { return a.begin < b.begin; });

  // Merge any 2 or more consecutive ranges where prev.end >= next.begin
  std::vector<intrange> out = {in.front()};
  for (auto &next : std::span(in.begin() + 1, in.end())) {
    auto &prev = out.back();
    if (prev.end >= next.begin) {
      // Overlap with previous range: extend the previous one to absorb this one
      prev.end = std::max(prev.end, next.end);
      continue;
    }
    // No overlap: append
    out.push_back(next);
  }

  return out;
}

// Appends the contents of src at the end of dst
template <typename T>
void extend(std::vector<T> &dst, std::vector<T> const &src) {
  std::size_t old_size = dst.size();
  dst.resize(old_size + src.size());
  std::copy(std::execution::unseq, src.begin(), src.end(),
            dst.begin() + static_cast<std::ptrdiff_t>(old_size));
}

} // namespace

std::uint64_t Day05::part1() {
  // Parsing
  auto rng = xmas::views::linewise(this->input);
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

std::uint64_t Day05::part2() {
  // Parsing
  auto rng = xmas::views::linewise(this->input);
  auto iline = rng.begin();

  // Parse header
  auto seed_pairs = parse_seeds(*iline);
  std::vector<intrange> seed_ranges;
  for (std::size_t i = 0; i + 1 < seed_pairs.size(); i += 2) {
    seed_ranges.push_back({
        .begin = seed_pairs[i],
        .end = seed_pairs[i] + seed_pairs[i + 1],
    });
  }

  xlog::debug("There are {} seeds", seed_pairs.size());

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

  // Computing each seed range in parallel
  return std::transform_reduce(
      std::execution::par_unseq, seed_ranges.cbegin(), seed_ranges.cend(),
      std::numeric_limits<std::uint64_t>::max(),
      [](std::uint64_t x, std::uint64_t y) { return x < y ? x : y; }, /* min */
      [&layers](const intrange seed_range) -> std::uint64_t {
        std::vector<intrange> in = {seed_range};
        for (auto const &layer : layers) {
          std::vector<intrange> out;
          for (auto &i : in) {
            extend(out, layer.translate(i));
          }

          in = coalesce_ranges(out);
        }
        const auto best = in.front().begin;

        xlog::debug("[{}, {}) -> {}", seed_range.begin, seed_range.end, best);
        return best;
      });
}
