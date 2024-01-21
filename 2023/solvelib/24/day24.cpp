#include "day24.hpp"

#include "xmaslib/iota/iota.hpp"
#include "xmaslib/log/log.hpp"
#include "xmaslib/parsing/parsing.hpp"
#include "xmaslib/line_iterator/line_iterator.hpp"
#include "xmaslib/matrix/dense_algebra.hpp"
#include "xmaslib/matrix/dense_vector.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <execution>
#include <format>
#include <functional>
#include <iterator>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace {

using Float = long double;
using Int = std::int64_t;
using Vector = xmas::basic_vector<Float>;

struct Line {
  // Defined as x(t) = origin + t * director
  xmas::basic_vector<Float> origin{};
  xmas::basic_vector<Float> director{};
};

constexpr bool almost_zero(Float v) {
  return std::abs(v) < 1e-8;
}

bool coplanar(Vector const& u, Vector const& v, Vector const& w) {
  Float determinant = 0;
  for (std::size_t i = 0; i < 3; ++i) {
    auto j = (i + 1) % 3;
    auto k = (j + 1) % 3;
    determinant += u[i] * (v[j] * w[k] - v[k] * w[j]);
  }

  return almost_zero(determinant);
}

std::optional<Float> intersection_with_YZ_plane(Line const& A, Float x) {
  if (almost_zero(A.director[0] - x)) {
    return {};
  }
  return {(x - A.origin[0]) / A.director[0]};
}

std::optional<Float> intersection_with_XZ_plane(Line const& A, Float y) {
  if (almost_zero(A.director[1] - y)) {
    return {};
  }
  return {(y - A.origin[1]) / A.director[1]};
}

bool sorted(Float x, Float y, Float z) {
  return (x <= y && y <= z);
}

bool line_enters_region_2D(Line const& A, Float min, Float max) {
  if (sorted(min, A.origin[0], max) && sorted(min, A.origin[1], max)) {
    return true;
  }

  auto k = intersection_with_YZ_plane(A, min);
  if (k.has_value() && *k > 0) {
    return true;
  }

  k = intersection_with_YZ_plane(A, max);
  if (k.has_value() && *k > 0) {
    return true;
  }

  k = intersection_with_XZ_plane(A, min);
  if (k.has_value() && *k > 0) {
    return true;
  }

  k = intersection_with_XZ_plane(A, max);
  if (k.has_value() && *k > 0) {
    return true;
  }

  return false;
}

/*
Algorithm
---------
This works in 3D as well, I didn't read the problem statement properly. Good thing is that what
works for 3D also works for 2D.
---
We need to:
- Find the intercept of two lines A:p+λu and B:q+κv
- This intercept may or may not exist
- Only intercepts in the future (κ>0 and λ>0) are accepted

Solution
1. Define plane Π:p+λu+αw, where w := q - p
2. x,p and q must draw a triangle embedded inside Π.
   Hence the edge vectors (x-p), (x-q), (q-p) must as well.
   These magnitues have names: λu, κv, and w.
   λ amd κ are scalars so we can pull them out. u,w,w must be coplanar
   We check this by ascertaining det[(u v w)] == 0
3. We define a basis for Π via Gramm-Schmitt, with origin at p
    O = p
    î = u / ||u||
    ĵ = w / || w || - (w · î)/||w|| î
4. Translate the two lines to this new basis (O, ξ, η)
   A: ξ =       λu·î   η =       κu·ĵ
   B: ξ = w·î + κv·î   η = w·ĵ + κv·ĵ
5. Find the intercept by setting A(λ)=B(κ)
    λu·î = w·î + κv·î }
    λu·ĵ = w·ĵ + κv·ĵ }
6. Note that u and ĵ are perpendicular (thanks Gramm-Schmitt), so the second equation becomes:
    0 = w·ĵ + κv·ĵ
6. Hence,
    κ = - (w·ĵ)/(v·ĵ)
    λ = (w + κv)·î / (u·î)
7. If κ or λ are negative, the intercepts are in the past
8. Ortherwise, we now know the intercept point to be:
    x = q+κv
*/
std::optional<std::pair<Float, Float>> find_intercept(Line const& A, Line const& B) {
  Vector const& u = A.director;
  Vector const& v = B.director;
  Vector const w = B.origin - A.origin;

  if (!coplanar(u, v, w)) {
    return {};
  }

  // Define a basis for the plane containing A and B
  Vector i = A.director.normalized();
  Vector j = (w - xmas::algebra::inner(w, i) * i).normalized();

  // Find intercept of A:p+λu and B:q+κv
  Float kappa = -xmas::algebra::inner(w, j) / xmas::algebra::inner(v, j);
  Float lambda = xmas::algebra::inner(w + kappa * v, i) / xmas::algebra::inner(u, i);

  return {
    {lambda, kappa}
  };
}

std::optional<Vector> find_future_intercept(Line const& A, Line const& B) {
  auto opt = find_intercept(A, B);

  if (!opt.has_value()) {
    return {};
  }

  auto [lambda, kappa] = *opt;
  if (lambda < 0 || kappa < 0) {
    return {};
  }

  return {B.origin + kappa * B.director};
}

Line parse_line_2D(std::string_view text) {
  auto ints = xmas::parse_ints<std::int64_t>(text);
  assert(ints.size() == 6);

  return {
    .origin = {Float(ints[0]), Float(ints[1]), Float(0.0)},
    .director = {Float(ints[3]), Float(ints[4]), Float(0.0)},
  };
}

}

std::uint64_t Day24::part1_generalized(std::int64_t min_xy, std::int64_t max_xy) {
  xmas::views::linewise in(this->input);

  auto min = Float(min_xy);
  auto max = Float(max_xy);

  std::vector<Line> lines;
  std::transform(in.cbegin(), in.cend(), std::back_inserter(lines), parse_line_2D);

  xmas::views::iota<std::size_t> iota(lines.size());

  return std::transform_reduce(std::execution::par_unseq, iota.begin(), iota.end(),
    std::uint64_t{0}, std::plus<std::uint64_t>{},
    [iota, &lines, min, max](std::size_t i) -> std::uint64_t {
      if (!line_enters_region_2D(lines[i], min, max)) {
        return 0;
      }

      return std::transform_reduce(iota.begin() + std::ptrdiff_t(i) + 1, iota.end(),
        std::uint64_t{0}, std::plus<std::uint64_t>{},
        [&i, &lines, min, max](std::size_t j) -> std::uint64_t {
          auto intersect = find_future_intercept(lines[i], lines[j]);
          if (!intersect.has_value()) {
            return 0;
          }

          Vector x = *intersect;

          if (x[0] < min || x[0] > max) {
            return 0;
          }

          if (x[1] < min || x[1] > max) {
            return 0;
          }

          xlog::debug("Detected in-bounds intersection between lines {} and {}", i, j);
          return 1;
        });
    });
}

std::uint64_t Day24::part1() {
  return part1_generalized(200000000000000, 400000000000000);
}

namespace {

Line parse_line_3D(std::string_view text) {
  auto ints = xmas::parse_ints<Int>(text);
  assert(ints.size() == 6);

  return {
    .origin = {Float(ints[0]), Float(ints[1]), Float(ints[2])},
    .director = {Float(ints[3]), Float(ints[4]), Float(ints[5])},
  };
}

template <typename T, std::integral U>
U cast_with_warning(T t) {
  U u = static_cast<U>(t + 0.5);
  if (std::isnan(t)) {
    xlog::warning("Bad conversion from {} to {}: NaN", typeid(T).name(), typeid(U).name());
  } else if (std::abs(static_cast<T>(u) - t) > 0.05) {
    xlog::warning("Bad conversion from {} to {}: {} != {}", typeid(T).name(), typeid(U).name(),
      (long double)t, u);
  }
  return u;
}

// closest_point finds the closest point in a line to the origin.
Vector closest_point(Line const& l) {
  auto vu = xmas::algebra::inner(l.origin, l.director);
  auto uu = l.director.norm2();
  return l.origin - (vu / uu) * l.director;
}

Vector cross_product_3D(Vector u, Vector v) {
  assert(u.size() == 3);
  assert(v.size() == 3);

  return {
    +(u[1] * v[2] - u[2] * v[1]),
    -(u[0] * v[2] - u[2] * v[0]),
    +(u[0] * v[1] - u[1] * v[0]),
  };
}

std::optional<Line> solve_part_2(Line const& reference, Line L1, Line L2) {
  // Use first line as reference
  L1.origin -= reference.origin;
  L1.director -= reference.director;

  L2.origin -= reference.origin;
  L2.director -= reference.director;

  // Find planes defined by origin and each line
  auto n1 = cross_product_3D(L1.director, closest_point(L1)).normalized();
  auto n2 = cross_product_3D(L2.director, closest_point(L2)).normalized();

  // Find line intersecting both planes
  auto u = cross_product_3D(n1, n2);
  if (almost_zero(u.norm2())) {
    // Lines are coplanar
    return {};
  }
  u.normalize();

  // Define a solution trajectory
  // This trajectory coincides with the solution's, but starts at the wrong spot
  // and moves at the wrong speed.
  Line traj{
    .origin = {0, 0, 0},
    .director = u,
  };

  // Find collision times
  auto opt = find_intercept(traj, L1);
  if (!opt.has_value()) {
    return {};
  }
  auto [k1, t1] = *opt;

  opt = find_intercept(traj, L2);
  if (!opt.has_value()) {
    return {};
  }
  auto [k2, t2] = *opt;

  // Scale director vector so speed is correct
  auto dk = k2 - k1;
  auto dt = t2 - t1;
  traj.director *= dk / dt;

  // Find collision point with line 1
  auto collision = L1.origin + t1 * L1.director;

  // We know collision point and velocity: rewind time to find the origin.
  traj.origin = collision - t1 * traj.director;

  // Undo change of reference
  return {
    {
     .origin = traj.origin + reference.origin,
     .director = traj.director + reference.director,
     }
  };
}
}

std::uint64_t Day24::part2() {
  xmas::views::linewise in(this->input);

  std::vector<Line> lines;
  std::transform(in.begin(), in.end(), std::back_inserter(lines), parse_line_3D);

  // Solve
  std::optional<Line> solution;
  // Looks O(n³) but we shortcut after very few iterations
  for (std::size_t i = 0; i < lines.size() && !solution.has_value(); ++i) {
    for (std::size_t j = i + 1; j < lines.size() && !solution.has_value(); ++j) {
      for (std::size_t k = j + 1; k < lines.size(); ++k) {
        solution = solve_part_2(lines[i], lines[j], lines[k]);

        if (solution.has_value()) {
          xlog::debug("Found solution with lines {}, {}, {}", i, j, k);
          break;
        }
        // Otherwise, the lines were coplanar, making the system indeterminate
      }
    }
  }

  if (!solution.has_value()) {
    throw std::runtime_error("System is indeterminate");
  }

  xlog::debug("Start position is {}", solution->origin.format("{:.2f}"));
  xlog::debug("Velocity is       {}", solution->director.format("{:.2f}"));

  // Generate answer
  auto u = solution->origin;
  return std::transform_reduce(u.begin(), u.end(), std::uint64_t{0}, std::plus<std::uint64_t>{},
    cast_with_warning<Float, std::uint64_t>);
}
