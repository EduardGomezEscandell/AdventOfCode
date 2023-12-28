#include "day10.hpp"
#include "xmaslib/functional/functional.hpp"
#include "xmaslib/log/log.hpp"
#include "xmaslib/matrix/text_matrix.hpp"
#include <algorithm>
#include <cassert>
#include <execution>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <vector>

namespace {
struct coords {
  std::size_t r;
  std::size_t c;
};

enum heading {
  N,
  E,
  S,
  W,
};

std::optional<heading> update_heading(
  xmas::views::text_matrix map, coords pos, heading curr_heading) {
  switch (map.at(pos)) {
  case 'F':
    switch (curr_heading) {
    case N:
      return E;
    case W:
      return S;
    default:
      return {};
    }
  case 'L':
    switch (curr_heading) {
    case S:
      return E;
    case W:
      return N;
    default:
      return {};
    }
  case '|':
    switch (curr_heading) {
    case N:
      return N;
    case S:
      return S;
    default:
      return {};
    }
  case '-':
    switch (curr_heading) {
    case E:
      return E;
    case W:
      return W;
    default:
      return {};
    }
  case '7':
    switch (curr_heading) {
    case E:
      return S;
    case N:
      return W;
    default:
      return {};
    }
  case 'J':
    switch (curr_heading) {
    case S:
      return W;
    case E:
      return N;
    default:
      return {};
    }
  case '.':
    return {};
  case 'S':
    return {};
  }

  throw std::runtime_error(std::format("unexpected cell with value {}", map.at(pos)));
}

std::optional<coords> take_step(xmas::views::text_matrix map, coords pos, heading heading) {
  // clang-format off
  switch (heading) {
  case N:
    if(pos.r == 0) return {};
    return {{pos.r - 1, pos.c}};
  case S:
    if(pos.r+1 == map.nrows()) return {};
    return {{pos.r + 1, pos.c}};
  case W:
    if(pos.c == 0) return {};
    return {{pos.r, pos.c - 1}};
  case E:
    if(pos.c+1 == map.ncols()) return {};
    return {{pos.r, pos.c + 1}};
  }
  // clang-format on

  throw std::runtime_error("Invalid value for heading");
}

std::vector<heading> find_start_directions(xmas::views::text_matrix map, coords start) {
  std::vector<heading> possible_headings;

  // North side
  if (start.r != 0) {
    if (update_heading(map, {start.r - 1, start.c}, N).has_value()) {
      possible_headings.push_back(N);
    }
  }

  // South side
  if (start.r + 1 != map.nrows()) {
    if (update_heading(map, {start.r + 1, start.c}, S).has_value()) {
      possible_headings.push_back(S);
    }
  }

  // East side
  if (start.c != 0) {
    if (update_heading(map, {start.r, start.c + 1}, E).has_value()) {
      possible_headings.push_back(E);
    }
  }

  // West side
  if (start.c != 0) {
    if (update_heading(map, {start.r, start.c - 1}, W).has_value()) {
      possible_headings.push_back(W);
    }
  }

  return possible_headings;
}

std::uint64_t loop_size(xmas::views::text_matrix map, coords start, heading heading) {
  assert(map.at(start) == 'S');

  std::uint64_t dist = 1;
  auto pos = take_step(map, start, heading);
  if (!pos.has_value()) {
    xlog::warning("Cancelling path after 0 steps");
    return {};
  }

  auto head = std::make_optional(heading);

  while (true) {

    head = update_heading(map, *pos, *head);
    if (!head.has_value()) {
      break;
    }

    pos = take_step(map, *pos, *head);
    if (!pos.has_value()) {
      return 0; // Path path
    }
    ++dist;
  }

  if (map.at(*pos) != 'S') {
    // Bad path
    return 0;
  }

  return dist / 2;
}

}

std::uint64_t Day10::part1() {
  xmas::views::text_matrix lines(this->input);

  auto p = std::ranges::find(this->input, 'S') - this->input.begin();
  coords S{
    .r = std::size_t(p) / (lines.ncols() + 1), // +1 for the \n
    .c = std::size_t(p) % (lines.ncols() + 1),
  };

  auto possible_headings = find_start_directions(lines, S);

  return std::transform_reduce(std::execution::par_unseq, possible_headings.begin(),
    possible_headings.end(), std::uint64_t{0}, xmas::max<std::uint64_t>{},
    [&](heading h) { return loop_size(lines, S, h); });
}

std::uint64_t Day10::part2() {
  throw std::runtime_error("Not implemented");
}
