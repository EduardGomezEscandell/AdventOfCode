#include "day24.hpp"
#include "xmaslib/iota/iota.hpp"
#include "xmaslib/log/log.hpp"
#include "xmaslib/matrix/dense_algebra.hpp"
#include "xmaslib/matrix/dense_matrix.hpp"
#include "xmaslib/matrix/dense_vector.hpp"
#include "xmaslib/parsing/parsing.hpp"
#include "xmaslib/line_iterator/line_iterator.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <execution>
#include <functional>
#include <iterator>
#include <numeric>
#include <optional>
#include <string_view>
#include <vector>

namespace {

using Float = double;
using Vector = xmas::basic_vector<Float>;
using Matrix = xmas::basic_matrix<Float>;

constexpr bool almost_zero(Float v) {
  return std::abs(v) < 1e-8;
}

struct line {
  // Defined as x=p+ku
  Vector origin;
  Vector director;
};

auto determinant3(Matrix const& A) {
  assert(A.nrows() == 3);
  assert(A.ncols() == 3);

  Float r = 0;
  Float sign = 1;
  for (std::size_t i = 0; i < 3; ++i) {
    auto j = (i + 1) % 3;
    auto k = (j + 1) % 3;

    r += sign * A[i][i] * (A[j][j] * A[k][k] - A[j][k] * A[k][j]);
    r *= -1;
  }

  return r;
}

bool coplanar(Vector const& u, Vector const& v, Vector const& w) {
  Matrix M(3, 3);
  M[0][0] = u[0];
  M[1][0] = u[1];
  M[2][0] = u[2];

  M[0][1] = v[0];
  M[1][1] = v[1];
  M[2][1] = v[2];

  M[0][2] = w[0];
  M[1][2] = w[1];
  M[2][2] = w[2];

  auto d = determinant3(M);
  return almost_zero(d);
}

std::optional<Float> intersection_with_YZ_plane(line const& A, Float x) {
  if (almost_zero(A.director[0] - x)) {
    return {};
  }
  return {(x - A.origin[0]) / A.director[0]};
}

std::optional<Float> intersection_with_XZ_plane(line const& A, Float y) {
  if (almost_zero(A.director[1] - y)) {
    return {};
  }
  return {(y - A.origin[1]) / A.director[1]};
}

bool sorted(Float x, Float y, Float z) {
  return (x <= y && y <= z);
}

bool line_enters_region_2D(line const& A, Float min, Float max) {
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
std::optional<Vector> find_intercept(line const& A, line const& B) {
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

  if (kappa < 0 || lambda < 0) {
    return {};
  }

  return {B.origin + kappa * B.director};
}

line parse_line_2D(std::string_view text) {
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

  std::vector<line> lines;
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
          auto intersect = find_intercept(lines[i], lines[j]);
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

std::uint64_t Day24::part2() {
  throw std::runtime_error("Not implemented");
}