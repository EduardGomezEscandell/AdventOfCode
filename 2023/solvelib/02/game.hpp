#pragma once

#include <cstdint>
#include <format>
#include <string_view>

struct rgb {
  std::int64_t red;
  std::int64_t green;
  std::int64_t blue;
};

struct game {
  std::int64_t id;
  rgb max;

  explicit game(std::string_view);
};

template <> struct std::formatter<rgb> : std::formatter<std::string> {
  auto format(rgb const &r, format_context &ctx) const {
    return formatter<std::string>::format(
        std::format("{{{}, {}, {} }}", r.red, r.green, r.blue), ctx);
  }
};

template <> struct std::formatter<game> : std::formatter<std::string> {
  auto format(game const &g, format_context &ctx) const {
    auto f = std::format("{{ game: {}, max: {}}}", g.id, g.max);

    return formatter<std::string>::format(f, ctx);
  }
};