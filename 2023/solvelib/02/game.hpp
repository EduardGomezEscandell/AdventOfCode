#pragma once

#include <cstdint>
#include <format>
#include <string_view>
#include <vector>

struct Round {
  std::int64_t red;
  std::int64_t green;
  std::int64_t blue;

  explicit Round(std::string_view);
  Round(std::int64_t r, std::int64_t g, std::int64_t b)
      : red(r), green(g), blue(b){};
  Round() = default;
};

struct game {
  std::int64_t id;
  std::vector<Round> rounds;

  explicit game(std::string_view);
};

template <> struct std::formatter<Round> : std::formatter<std::string> {
  auto format(Round const &r, format_context &ctx) const {
    return formatter<std::string>::format(
        std::format("{{{}, {}, {} }}", r.red, r.green, r.blue), ctx);
  }
};

template <> struct std::formatter<game> : std::formatter<std::string> {
  auto format(game const &g, format_context &ctx) const {
    auto f = std::format("Game {}: ", g.id);
    if (g.rounds.size() != 0) {
      f += std::format("{}", g.rounds[0]);
    }
    for (std::size_t i = 1; i < g.rounds.size(); ++i) {
      f += std::format(", {}", g.rounds[i]);
    }
    f += "]}";

    return formatter<std::string>::format(f, ctx);
  }
};