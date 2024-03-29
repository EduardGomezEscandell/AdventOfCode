#include <numeric>
#include <string_view>

#include "day02.hpp"
#include "game.hpp"
#include "xmaslib/line_iterator/line_iterator.hpp"
#include "xmaslib/log/log.hpp"

std::uint64_t Day02::part1() {
  xmas::views::linewise range(this->input);

  constexpr std::int64_t max_red = 12;
  constexpr std::int64_t max_green = 13;
  constexpr std::int64_t max_blue = 14;

  return std::transform_reduce(
    range.begin(), range.end(), 0u, std::plus{}, [](std::string_view line) -> std::int64_t {
      const game g(line);

      if (g.max.red > max_red) {
        xlog::debug("Rejected because of red: {}", g);
        return 0;
      }
      if (g.max.green > max_green) {
        xlog::debug("Rejected because of green: {}", g);
        return 0;
      }
      if (g.max.blue > max_blue) {
        xlog::debug("Rejected because of blue: {}", g);
        return 0;
      }

      return g.id;
    });
}

std::uint64_t Day02::part2() {
  xmas::views::linewise range(this->input);

  return std::transform_reduce(
    range.begin(), range.end(), 0u, std::plus{}, [](std::string_view line) -> std::uint64_t {
      const game g(line);

      const auto p = static_cast<std::uint64_t>(g.max.red * g.max.green * g.max.blue);
      xlog::debug("Power value is {} for {}", p, g);
      return p;
    });
}
