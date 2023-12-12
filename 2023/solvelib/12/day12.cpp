#include "day12.hpp"

#include "xmaslib/functional/functional.hpp"
#include "xmaslib/iota/iota.hpp"
#include "xmaslib/lazy_string/lazy_string.hpp"
#include "xmaslib/line_iterator/line_iterator.hpp"
#include "xmaslib/log/log.hpp"
#include "xmaslib/lru/lru.hpp"
#include "xmaslib/parsing/parsing.hpp"
#include "xmaslib/view/view.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <execution>
#include <functional>
#include <iterator>
#include <numeric>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace {

enum class cell : char { unknown = '?', set = '#', empty = '.' };

auto parse_line(std::string_view line) {
  auto separator = std::ranges::find(line, ' ');
  if (separator == line.end()) {
    throw std::runtime_error(std::format("Line '%s' has no separator", line));
  }

  // Cheeky fast copy (only works because cells are a typedef'd char)
  auto const size = static_cast<std::size_t>(separator - line.begin());
  std::vector<cell> cells(size);
  ::memcpy(cells.data(), line.data(), size);

  // Numbers
  auto ranges = xmas::parse_ints<int>({separator + 1, line.cend()});

  return std::make_pair(cells, ranges);
}

std::tuple<std::size_t, std::size_t, bool> trim(auto cells_base,
                                                auto ranges_base) {
  auto cells = xmas::view{cells_base.begin(), cells_base.end()};
  auto ranges = xmas::view{ranges_base.begin(), ranges_base.end()};

  while (true) {
    if (ranges.size() == 0) {
      break;
    }

    // Trim whitespace (..XXX,)  -> (XXX,)
    auto begin = std::ranges::find_if_not(cells, xmas::equals(cell::empty));
    cells.pop_front(begin - cells.begin());

    if (cells.size() == 0) {
      // Nothing left to trim
      return {{}, {}, ranges.size() == 0};
    }

    if (cells.front() == cell::unknown) {
      // No more trimming possible
      break;
    }

    if (static_cast<std::size_t>(ranges.front()) > cells.size()) {
      // Bad! Next range does not fit
      return {{}, {}, false};
    }

    // Trimming prefixed ###
    auto it =
        std::find_if_not(cells.begin(), cells.end(), xmas::equals(cell::set));

    auto const prefix_size = it - cells.begin();
    if (prefix_size > ranges.front()) {
      // Bad! the prefix is longer than the range
      return {{}, {}, false};
    }

    // The prefix is the entire string
    if (it == cells.end()) {
      if (ranges.front() != prefix_size) {
        // Bad! the prefix does not match the range
        return {{}, {}, false};
      }

      if (ranges.size() > 1) {
        // Bad! Leftover ranges with no more cells
        return {{}, {}, false};
      }

      cells.pop_front(cells.size());
      ranges.pop_front();
      break;
    }

    // The prefix is followed by a .
    if (*it == cell::empty) {
      if (ranges.front() != prefix_size) {
        // Bad! the prefix does not match the range
        return {{}, {}, false};
      }

      cells.pop_front(prefix_size + 1); // +1 for the .
      ranges.pop_front();
      continue;
    }

    // The prefix is followed by ?
    if (prefix_size == ranges.front()) {
      // The prefix matches the range exactly
      cells.pop_front(prefix_size + 1); // +1 because the next ? must be a .
      ranges.pop_front();
      continue;
    }

    // The prefix is shorter than the range: force ? into # to match the
    // expected range
    auto rng = cells.take(ranges.front());
    it = std::ranges::find(rng, cell::empty);
    auto const prefixable_size = it - rng.begin();

    if (it != rng.end()) {
      // Bad! Range does not fit
      return {{}, {}, false};
    }

    // Entire string is prefixable
    if (it == cells.end()) {
      if (ranges.size() != 1) {
        // Bad! Leftover ranges with no more cells
        return {{}, {}, false};
      }

      cells.pop_front(cells.size());
      ranges.pop_front();
      break;
    }

    if (*it == cell::set) {
      // Bad! collision with next range
      return {{}, {}, false};
    }

    cells.pop_front(prefixable_size +
                    1); // +1 because next . or ? is forced into .
    ranges.pop_front();
  }

  return {cells_base.size() - cells.size(), ranges_base.size() - ranges.size(),
          true};
}

auto stringify_line(auto const &cells, auto const &ranges) {
  return xmas::lazy_string{[=]() {
    std::stringstream ss;
    ss << std::string_view{reinterpret_cast<char const *>(&cells.front()),
                           cells.size()};
    ss << " ";
    if (ranges.size() == 0) {
      return ss.str();
    }
    for (int x : ranges) {
      ss << x << ",";
    }
    auto s = std::move(ss).str();
    s.pop_back(); // trailing comma
    return s;
  }};
}

auto trim_left(xmas::view<std::vector<cell>::const_iterator> cells,
               xmas::view<std::vector<int>::const_iterator> ranges) {
  auto const [ctrim, rtrim, ok] = trim(cells, ranges);
  cells.pop_front(ctrim);
  ranges.pop_front(rtrim);

  return std::make_tuple(std::move(cells), std::move(ranges), ok);
}

auto trim_right(xmas::view<std::vector<cell>::const_iterator> cells,
                xmas::view<std::vector<int>::const_iterator> ranges) {

  auto const [ctrim, rtrim, ok] = trim(std::ranges::views::reverse(cells),
                                       std::ranges::views::reverse(ranges));
  cells.pop_back(ctrim);
  ranges.pop_back(rtrim);

  return std::make_tuple(std::move(cells), std::move(ranges), ok);
}

bool contains(xmas::view<std::vector<cell>::const_iterator> v, cell c) {
  auto it = std::find(v.begin(), v.end(), c);
  return it != v.end();
}

struct state {
  xmas::view<std::vector<cell>::const_iterator> cells;
  xmas::view<std::vector<int>::const_iterator> ranges;

  bool operator==(state const &other) const noexcept {
    return (&*cells.begin() == &*other.cells.begin()) &&
           (cells.size() == other.cells.size()) &&
           (&*ranges.begin() == &*other.ranges.begin()) &&
           (cells.size() == other.cells.size());
  }

  std::size_t hash() const noexcept {
    static_assert(sizeof(std::size_t) >= 8);
    auto h1 = hash16bit(&*cells.begin());
    auto h2 = hash16bit(&*cells.end());
    auto h3 = hash16bit(&*ranges.begin());
    auto h4 = hash16bit(&*ranges.end());
    return h1 << 48 | h2 << 32 | h3 << 16 | h4; // or use boost::hash_combine
  }

private:
  static std::size_t hash16bit(auto const *p) {
    return std::hash<std::uint64_t>{}(reinterpret_cast<std::uint64_t>(p)) &
           0xffff;
  }
};

} // namespace

template <> struct std::hash<state> {

  std::size_t operator()(const state &s) const noexcept { return s.hash(); }
};

namespace {

[[nodiscard]] std::uint64_t
count_combinations(xmas::lru_cache<state, std::uint64_t> &lru,
                   xmas::view<std::vector<cell>::const_iterator> cells_base,
                   xmas::view<std::vector<int>::const_iterator> ranges_base) {

  if (auto opt = lru.get({cells_base, ranges_base}); opt.has_value()) {
    return *opt;
  }

  const auto result = [&]() -> std::uint64_t {
    // Deterministic progress
    //
    //   We consume # and . from left to write
    auto [cells, ranges, ok] = trim_left(cells_base, ranges_base);
    if (!ok) {
      return 0;
    }

    if (ranges.size() == 0) {
      if (contains(cells, cell::set)) {
        return 0;
      }
      return 1;
    }

    const auto leftover =
        static_cast<std::size_t>(std::reduce(ranges.begin(), ranges.end())) +
        ranges.size() - 1;
    if (cells.size() < leftover) {
      return 0;
    }

    // Speculative progress
    //
    // We try to force ? into # and . and see if it breaks
    auto pound = std::ranges::find(cells, cell::set);
    auto size = std::min(1 + cells.size() - static_cast<std::size_t>(leftover),
                         1 + static_cast<std::size_t>(pound - cells.begin()));

    const auto L = static_cast<std::size_t>(ranges.front());
    xmas::views::iota<std::size_t> iota(size);
    return std::transform_reduce(
        iota.begin(), iota.end(), std::uint64_t{0u}, std::plus<std::uint64_t>{},
        [&lru, cells, ranges, L](std::size_t idx) -> std::uint64_t {
          if (cells[idx] == cell::empty) {
            return 0;
          }

          if (idx != 0 && cells[idx - 1] == cell::set) {
            // Collision with prev block
            return 0;
          }

          auto v = cells.drop(idx);

          auto prefix = v.take(L);
          auto it = std::ranges::find(prefix, cell::empty);
          if (it != prefix.end()) {
            // Range does not fit
            return 0;
          }

          if (it != v.end() && *it == cell::set) {
            // Collision with next block
            return 0;
          }

          if (ranges.size() == 1) {
            // End of sequence
            if (contains(v.drop(ranges.front()), cell::set)) {
              return 0;
            }
            return 1;
          }

          auto x = count_combinations(lru, v.drop(L + 1), ranges.drop(1));
          return x;
        });
  }();

  lru.set(state{cells_base, ranges_base}, result);
  return result;
}

template <bool quintuple = false>
std::uint64_t process_line(std::string_view line) {
  auto [cells_base, ranges_base] = parse_line(line);
  if (ranges_base.size() == 0) {
    return 1;
  }

  if constexpr (quintuple) {
    auto cells0 = cells_base;
    auto ranges0 = ranges_base;

    cells_base.reserve(cells_base.size() * 5 + 4);
    ranges_base.reserve(ranges_base.size() * 5);

    for (std::size_t i = 0; i < 4; ++i) {
      cells_base.push_back(cell::unknown);
      std::copy(cells0.begin(), cells0.end(), std::back_inserter(cells_base));
      std::copy(ranges0.begin(), ranges0.end(),
                std::back_inserter(ranges_base));
    }
  }

  // Trim right. Left trim is done inside count_combinations
  auto [cells, ranges, ok] =
      trim_right(xmas::view{cells_base.cbegin(), cells_base.cend()},
                 xmas::view{ranges_base.cbegin(), ranges_base.cend()});

  if (!ok) {
    xlog::warning("No combinations possible for line {} (error from the right)",
                  line);
    return 0;
  }

  xmas::lru_cache<state, std::uint64_t> lru(128);

  const auto x = count_combinations(lru, cells, ranges);
  if (x == 0) {
    xlog::warning("No combinations possible for line {} (error from the left)",
                  line);
  } else {
    xlog::debug("{} -> ({}) -> {}", line, stringify_line(cells, ranges), x);
  }

  return x;
}

} // namespace

std::uint64_t Day12::part1() {
  auto in = xmas::views::linewise(this->input);

  return std::transform_reduce(std::execution::par_unseq, in.cbegin(),
                               in.cend(), std::uint64_t(0u),
                               std::plus<std::uint64_t>{}, process_line<false>);
}

std::uint64_t Day12::part2() {
  auto in = xmas::views::linewise(this->input);

  return std::transform_reduce(std::execution::par_unseq, in.cbegin(),
                               in.cend(), std::uint64_t(0u),
                               std::plus<std::uint64_t>{}, process_line<true>);
}