#include "day06.hpp"

#include "xmaslib/line_iterator/line_iterator.hpp"
#include "xmaslib/log/log.hpp"
#include "xmaslib/parsing/parsing.hpp"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <execution>
#include <functional>
#include <numeric>
#include <string_view>

namespace {

std::uint64_t parse_int_ignore_nonnumeric(std::string_view str) {
  std::uint64_t x{};
  for (auto ch : str) {
    if (ch < '0' || ch > '9') {
      continue;
    }
    x *= 10;
    x += static_cast<std::uint64_t>(ch - '0');
  }
  return x;
}

std::uint64_t count_wins(std::uint64_t time, std::uint64_t distance) noexcept {
  // Solve quadratic equation:
  //
  //    d=t(T - t)
  //
  //  > d is the distance+1 (aka the minimum distance to win)
  //  > T is the race duration
  //  > t is the charging time.
  //  The equation simplifies down:
  //
  //  t = -(T/2) ± sqrt((T/2)^2 - d)
  auto T = static_cast<double>(time);
  auto D = static_cast<double>(distance) + 1;

  double half = T / 2;
  double delta = std::sqrt(half * half - D);
  auto min_t = std::uint64_t(std::ceil(half - delta));
  auto max_t = std::uint64_t(std::floor(half + delta));

  xlog::debug("Race {},{} -> [{}, {}]", time, distance, int(min_t), int(max_t));
  return max_t - min_t + 1;
}

} // namespace

std::uint64_t Day06::part1() {
  xmas::views::linewise lines(input);

  auto it = lines.begin();
  auto times = xmas::parse_ints<std::uint64_t>(*it++);
  auto distances = xmas::parse_ints<std::uint64_t>(*it);

  assert(times.size() == distances.size());

  // It's faster with std::execution::unseq than with std::execution::par_unseq.
  // Probably because the input is too small to justify allocating extra
  // threads.
  return std::transform_reduce(
      std::execution::unseq, times.cbegin(), times.cend(), distances.cbegin(),
      std::uint64_t{1}, std::multiplies<std::uint64_t>{}, count_wins);
}

std::uint64_t Day06::part2() {
  xmas::views::linewise lines(input);
  auto it = lines.begin();
  auto time = parse_int_ignore_nonnumeric(*it++);
  auto distance = parse_int_ignore_nonnumeric(*it);
  return count_wins(time, distance);
}
