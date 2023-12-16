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
#include <stdexcept>

std::uint64_t Day06::part1() {
  xmas::views::linewise lines(input);

  auto it = lines.begin();
  auto times = xmas::parse_ints<std::uint64_t>(*it++);
  auto distances = xmas::parse_ints<std::uint64_t>(*it);

  assert(times.size() == distances.size());

  // It's faster with std::execution::unseq than with std::execution::par_unseq.
  // Probably because the input is too small to justify allocating extra threads.
  return std::transform_reduce(
      std::execution::unseq, times.cbegin(), times.cend(), distances.cbegin(),
      std::uint64_t{1}, std::multiplies<std::uint64_t>{},
      [](std::uint64_t time, std::uint64_t distance) {
        auto T = static_cast<float>(time);
        auto D = static_cast<float>(distance) + 1;

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

        float half = T / 2;
        float delta = std::sqrt(half * half - D);
        auto min_t = uint(half - delta);
        auto max_t = uint(half + delta);

        // These are lower bounds, we may need a correction
        auto d = min_t * (time - min_t);
        if (d <= distance) {
          ++min_t;
        }

        d = max_t * (time - max_t);
        if (d <= distance) {
          --max_t;
        }

        xlog::debug("Race {},{} -> [{}, {}]", T, D, int(min_t), int(max_t));
        return max_t - min_t + 1;
      });
}

std::uint64_t Day06::part2() { throw std::runtime_error("Not implemented"); }
