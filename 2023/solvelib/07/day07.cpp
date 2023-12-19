#include "day07.hpp"

#include "xmaslib/iota/iota.hpp"
#include "xmaslib/log/log.hpp"
#include "xmaslib/parsing/parsing.hpp"
#include "xmaslib/line_iterator/line_iterator.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <execution>
#include <format>
#include <functional>
#include <iterator>
#include <numeric>
#include <span>
#include <stdexcept>
#include <string_view>

namespace {

enum type {
  penta = 6,
  quadra = 5,
  full_house = 4,
  triple = 3,
  two_pair = 2,
  pair = 1,
  high = 0,
};

// Score is encoded in a 32-bit number. From right to left:
// Bits  0 to  4: Card 5
// Bits  5 to  8: Card 4
// Bits  9 to 12: Card 3
// Bits 13 to 16: Card 2
// Bits 17 to 20: Card 1
// Bits 21 to 24: Hand type
//
// This way, sorting scores sorts the hands
//
// Example hand [32T3K] has a pair (score = 0x1), and cards with values 3,2,10,3,13:
// score: 0x132a3d
//
// In part 1, J is encoded as 0xb. In part 2 it is encoded as 0x0 to ensure it is at the
// bottom of the priority ranking.
using score_t = std::uint32_t;

using hand = std::span<const char, 5>;

template <bool is_part_2>
std::uint32_t encode_card(char card) {
  if (card <= '9') {
    return static_cast<std::uint32_t>(card - '0');
  }
  switch (card) {
  case 'T':
    return 0xa;
  case 'J':
    if constexpr (is_part_2) {
      return 0;
    }
    return 0xb;
  case 'Q':
    return 0xc;
  case 'K':
    return 0xd;
  case 'A':
    return 0xe;
  [[unlikely]] default:
    throw std::runtime_error("Could not parse hand");
  }
}

constexpr std::array<std::uint8_t, 0xf> empty_score_array = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // No clean way of doing this apparently

template <bool is_part_2>
type compute_type(hand hand) {
  auto counts = empty_score_array;
  for (char card : hand) {
    ++counts[encode_card<is_part_2>(card)];
  }

  auto joker_count = std::exchange(counts[0], 0); // Will be 0 when is_part2 == false
  std::partial_sort(std::execution::unseq, counts.begin(), counts.begin() + 2, counts.end(),
    std::greater<std::uint8_t>{});
  counts[0] += joker_count;

  switch (counts[0]) {
  case 5:
    return penta;
  case 4:
    return quadra;
  case 3:
    return counts[1] == 2 ? full_house : triple;
  case 2:
    return counts[1] == 2 ? two_pair : pair;
  case 1:
    return high;
  [[unlikely]] default:
    throw std::runtime_error("Could not parse hand");
  }
}

template <bool is_part_2>
score_t hand_score(hand hand) {
  score_t score = compute_type<is_part_2>(hand);
  for (auto const& card : hand) {
    score <<= 4;
    score += encode_card<is_part_2>(card);
  }
  return score;
}

using player_t = std::uint64_t;

template <bool is_part_2>
player_t encode_player(std::string_view line) {
  std::uint64_t score = hand_score<is_part_2>(std::span<const char, 5>(line.begin(), 5));
  std::uint64_t bet = xmas::parse_int<std::uint32_t>({line.begin() + 6, line.end()});

  auto r = score << 32 | bet;
  xlog::debug("Player {} encoded as {:>x}", line, r);

  return r;
}

std::uint64_t extract_bid(player_t player) {
  return static_cast<score_t>(player & 0xffffffff);
}

template <bool is_part_2>
std::uint64_t solve(std::string_view input) {
  std::vector<player_t> players;

  auto lines = xmas::views::linewise(input);
  std::transform(lines.begin(), lines.end(), std::back_inserter(players), encode_player<is_part_2>);

  std::sort(players.begin(), players.end(), std::less<player_t>{});

  xmas::views::iota<std::uint64_t> position(1, 1 + players.size());
  return std::transform_reduce(std::execution::par_unseq, players.begin(), players.end(),
    position.begin(), std::uint64_t{0}, std::plus<std::uint64_t>{},
    [](player_t player, std::uint64_t position) -> std::uint64_t {
      return position * extract_bid(player);
    });
}
}

std::uint64_t Day07::part1() {
  return solve<false>(input);
}

std::uint64_t Day07::part2() {
  return solve<true>(input);
}
