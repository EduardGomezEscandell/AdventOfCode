#include "game.hpp"

#include <algorithm>
#include <execution>
#include <format>
#include <stdexcept>

#include "xmaslib/log/log.hpp"

Round::Round(std::string_view data) : Round{0, 0, 0} {
  auto it = data.begin();
  while (it != data.end()) {

    // Skip leading whitespace
    it = std::find_if(it, data.end(), [](auto ch) { return ch != ' '; });
    if (it == data.end())
      break;

    // Parse number
    std::int64_t count;
    const auto result = std::from_chars(it, data.end(), count, 10);
    if (result.ptr == data.end()) {
      throw std::runtime_error(
          std::format("could not parse dice count in string {}", data));
    }

    // Skip whitespace
    it =
        std::find_if(result.ptr, data.end(), [](auto ch) { return ch != ' '; });

    // Parse colour
    if (it == data.end())
      throw std::runtime_error(
          std::format("could not find colour in string {}", data));

    switch (*it) {
    case 'r':
      this->red += count;
      break;
    case 'g':
      this->green += count;
      break;
    case 'b':
      this->blue += count;
      break;
    default:
      throw std::runtime_error(std::format(
          "unknown colour in string {} (after digit {})", data, count));
    }

    // Advance start of next dice
    it = std::find(it, data.end(), ',');
    if (it != data.end()) {
      ++it;
    }
  }
}

game::game(std::string_view line) {
  // Parse prefix
  auto space = std::ranges::find(line, ' ');
  if (space == line.end())
    throw std::runtime_error(
        std::format("could not find space in line {}", line));

  const auto result = std::from_chars(space + 1, line.end(), this->id, 10);
  if (result.ptr == line.end()) {
    throw std::runtime_error(
        std::format("could not parse game ID in line {}", line));
  }

  // Parse rounds
  std::vector<std::string_view::const_iterator> semicolons;
  std::string_view::const_iterator it =
      result.ptr + 1; // +1 to ignore the whitespace after :
  semicolons.push_back(it);
  while (it != line.end()) {
    it = std::find(it + 1, line.end(), ';');
    semicolons.push_back(it);
  }

  try {
    this->rounds.resize(semicolons.size() - 1, Round{});
    std::transform(std::execution::par_unseq, semicolons.cbegin(),
                   semicolons.cend() - 1, semicolons.cbegin() + 1,
                   this->rounds.begin(), [](auto prev, auto next) -> Round {
                     return Round(std::string_view{prev + 1, next});
                   });
  } catch (std::runtime_error &e) {
    throw std::runtime_error(
        std::format("could not parse round in line {}: {}", line, e.what()));
  }
}