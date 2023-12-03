#include <numeric>
#include <string_view>

#include "day02.hpp"
#include "game.hpp"
#include "xmaslib/line_iterator/line_iterator.hpp"
#include "xmaslib/log/log.hpp"

std::int64_t Day02::part1() {
  xmas::line_range range(this->input);

  constexpr std::int64_t max_red = 12;
  constexpr std::int64_t max_green = 13;
  constexpr std::int64_t max_blue = 14;

  return std::transform_reduce(
      range.begin(), range.end(), 0, std::plus<std::int64_t>{},
      [](std::string_view line) -> std::int64_t {
        const game g(line);
        auto max_colours =
            std::reduce(g.rounds.begin(), g.rounds.end(), Round{},
                        [](Round const &acc, Round const &r) {
                          return Round(std::max(acc.red, r.red),
                                       std::max(acc.green, r.green),
                                       std::max(acc.blue, r.blue));
                        });

        if (max_colours.red > max_red) {
          xlog::debug("Rejected because of red: {}", g);
          return 0;
        }
        if (max_colours.green > max_green) {
          xlog::debug("Rejected because of green: {}", g);
          return 0;
        }
        if (max_colours.blue > max_blue) {
          xlog::debug("Rejected because of blue: {}", g);
          return 0;
        }

        return g.id;
      });
}

std::int64_t Day02::part2() {
  xmas::line_range range(this->input);

  return std::transform_reduce(
      range.begin(), range.end(), 0, std::plus<std::int64_t>{},
      [](std::string_view line) -> std::int64_t {
        const game g(line);

        auto max_colours =
            std::reduce(g.rounds.begin(), g.rounds.end(), Round{},
                        [](Round const &acc, Round const &r) {
                          return Round(std::max(acc.red, r.red),
                                       std::max(acc.green, r.green),
                                       std::max(acc.blue, r.blue));
                        });

        const auto p = max_colours.red * max_colours.green * max_colours.blue;
        xlog::debug("Power value is {} for {}", p, g);
        return p;
      });
}
