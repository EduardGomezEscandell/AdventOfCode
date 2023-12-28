#include "day10.hpp"

#include "xmaslib/log/log.hpp"
#include "xmaslib/matrix/text_matrix.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <execution>
#include <functional>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <vector>

namespace {

using Int = std::int64_t;

struct coords {
  Int r;
  Int c;
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
    if(std::size_t(pos.r+1) == map.nrows()) return {};
    return {{pos.r + 1, pos.c}};
  case W:
    if(pos.c == 0) return {};
    return {{pos.r, pos.c - 1}};
  case E:
    if(std::size_t(pos.c+1) == map.ncols()) return {};
    return {{pos.r, pos.c + 1}};
  }
  // clang-format on

  throw std::runtime_error("Invalid value for heading");
}

heading find_start_direction(xmas::views::text_matrix map, coords start) {
  std::vector<heading> possible_headings;

  // North side
  if (start.r != 0) {
    if (update_heading(map, {start.r - 1, start.c}, N).has_value()) {
      possible_headings.push_back(N);
    }
  }

  // South side
  if (std::size_t(start.r + 1) != map.nrows()) {
    if (update_heading(map, {start.r + 1, start.c}, S).has_value()) {
      possible_headings.push_back(S);
    }
  }

  // West side
  if (start.c != 0) {
    if (update_heading(map, {start.r, start.c - 1}, W).has_value()) {
      possible_headings.push_back(W);
    }
  }

  // East side
  if (std::size_t(start.c + 1) != map.ncols()) {
    if (update_heading(map, {start.r, start.c + 1}, E).has_value()) {
      possible_headings.push_back(E);
    }
  }

  switch (possible_headings.size()) {
  case 0:
    throw std::runtime_error("No possible heading from start point");
  case 1:
    throw std::runtime_error("No closed loop: only one direction available from start");
  case 2:
    break;
  default:
    xlog::warning("Multiple possible loops");
  }

  return possible_headings.front();
}

Int compute_perimeter(std::vector<coords> const& vertices) {
  return std::transform_reduce(std::execution::par_unseq, vertices.cbegin(), vertices.cend() - 1,
    vertices.cbegin() + 1, 0, std::plus<Int>{},
    [](coords const& u, coords const& v) { return std::abs(u.r - v.r) + std::abs(u.c - v.c); });
}

// Shoelace formula to find the (maybe non-integer) area, so 2*area is returned.
// https://en.wikipedia.org/wiki/Shoelace_formula#Shoelace_formula
Int compute_double_area(std::vector<coords> const& vertices) {
  Int twice_area = std::transform_reduce(std::execution::par_unseq, vertices.begin(),
    vertices.end() - 1, vertices.begin() + 1, Int{0}, std::plus<Int>{},
    [](coords const& u1, coords const& u2) { return u1.r * u2.c - u1.c * u2.r; });

  // Shoelace formula returns the signed area: the sign depends on wether the polygon is traversed
  // clockwise or counterclockise. This is not something we care about here, so we remove the sign.
  return std::abs(twice_area);
}

std::vector<coords> loop_path(xmas::views::text_matrix map, coords start, heading heading) {
  assert(map.at(start) == 'S');
  std::vector<coords> vertices{start};

  auto pos = take_step(map, start, heading);
  if (!pos.has_value()) {
    xlog::warning("Cancelling path after 0 steps");
    return {};
  }

  while (true) {
    auto new_heading = update_heading(map, *pos, heading);
    if (!new_heading.has_value()) {
      break;
    }

    if (*new_heading != heading) {
      heading = *new_heading;
      vertices.push_back(*pos);
    }

    pos = take_step(map, *pos, heading);
    if (!pos.has_value()) {
      return {}; // Bad path
    }
  }

  if (map.at(*pos) != 'S') {
    // Bad path
    return {};
  }

  vertices.push_back(start);

  return vertices;
}

Int loop_perimeter(xmas::views::text_matrix map, coords start, heading heading) {
  auto vertices = loop_path(map, start, heading);
  if (vertices.empty()) {
    return 0; // Bad path
  }

  return compute_perimeter(vertices);
}

Int loop_enclosed_area(xmas::views::text_matrix map, coords start, heading heading) {
  auto vertices = loop_path(map, start, heading);
  if (vertices.empty()) {
    return 0; // Bad path
  }
  vertices.push_back(vertices.front());

  Int twice_area = compute_double_area(vertices);
  Int perimeter = compute_perimeter(vertices);

  // Pick's theorem
  // A = i + b/2 - 1
  //
  // where A is the area, i is the count of internal points, and
  // b is the count of boundary points. We can re-arrange it to:
  //
  // i = (2A - b)/2 + 1
  return (twice_area - perimeter) / 2 + 1;
}

}

std::uint64_t Day10::part1() {
  xmas::views::text_matrix lines(this->input);

  auto p = std::ranges::find(this->input, 'S') - this->input.begin();
  coords S{
    .r = Int(p) / Int(lines.ncols() + 1), // +1 for the \n
    .c = Int(p) % Int(lines.ncols() + 1),
  };

  auto heading = find_start_direction(lines, S);

  Int perimeter = loop_perimeter(lines, S, heading);
  return static_cast<std::uint64_t>(perimeter / 2);
}

std::uint64_t Day10::part2() {
  xmas::views::text_matrix lines(this->input);

  auto p = std::ranges::find(this->input, 'S') - this->input.begin();
  coords S{
    .r = Int(p) / Int(lines.ncols() + 1), // +1 for the \n
    .c = Int(p) % Int(lines.ncols() + 1),
  };

  auto heading = find_start_direction(lines, S);
  Int enclosed_area = loop_enclosed_area(lines, S, heading);
  return static_cast<std::uint64_t>(enclosed_area);
}
