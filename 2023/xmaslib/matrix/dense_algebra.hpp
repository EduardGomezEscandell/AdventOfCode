#pragma once

#include "xmaslib/log/log.hpp"
#include "xmaslib/matrix/dense_matrix.hpp"
#include <concepts>
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <numeric>
#include <execution>
#include <type_traits>

namespace xmas {

template <typename T>
class basic_vector;

template <typename T>
class dense_matrix;

namespace algebra {

auto inner(auto const& lhs, auto const& rhs) {
  using T = std::remove_cvref_t<decltype(*lhs.begin())>;
  return std::transform_reduce(std::execution::unseq, lhs.begin(), lhs.end(), rhs.begin(), T{0},
    std::plus<T>{}, std::multiplies<T>{});
}

template <typename T>
T inner(basic_vector<T> const& lhs, auto const& rhs) {
  return inner(lhs.data(), rhs);
}

template <typename T>
T inner(basic_vector<T> const& lhs, basic_vector<T> const& rhs) {
  return inner(lhs, rhs.data());
}

template <typename T>
dense_matrix<T> outter(auto const& lhs, auto const& rhs) {
  dense_matrix<T> out(lhs.size(), rhs.size());
  for (std::size_t i = 0; i < lhs.size(); ++i) {
    for (std::size_t j = 0; j < rhs.size(); ++j) {
      out[i][j] = lhs[i] * rhs[j];
    }
  }
  return out;
}

template <typename T>
auto outter(basic_vector<T> const& lhs, auto const& rhs) {
  return outter<T>(lhs.data(), rhs);
}

template <typename T>
auto outter(basic_vector<T> const& lhs, basic_vector<T> const& rhs) {
  return outter<T>(lhs.data(), rhs.data());
}

template <typename T, typename Matrix, typename Vector>
basic_vector<T> MV_mult(Matrix const& A, Vector const& u) {
  assert(A.ncols() == u.size());
  basic_vector<T> out(A.nrows());
  std::transform(std::execution::par_unseq, A.crows().begin(), A.crows().end(), out.begin(),
    [&u](auto const& row) { return inner(row, u); });
  return out;
}
}

// Matrix-vector product
template <typename Matrix, typename Vector>
auto operator*(Matrix const& A, Vector const& u)
  requires(!std::same_as<decltype(*A.begin()), decltype(*u.begin())>)
{
  using T = decltype(*u.begin());
  return algebra::MV_mult<T>(A, u);
}

template <typename T, typename Matrix>
auto operator*(Matrix const& A, basic_vector<T> const& u)
  requires(is_matrix<Matrix, T>)
{
  return algebra::MV_mult<T>(A, u);
}

// Vector-matrix product
template <typename T>
basic_vector<T> operator*(auto const& u, is_matrix<T> auto const& A) {
  return algebra::MV_mult<T>(A.transpose(), u);
}

template <typename T>
basic_vector<T> operator*(basic_vector<T> const& u, is_matrix<T> auto const& A) {
  return algebra::MV_mult<T>(A.transpose(), u);
}

namespace algebra {
// Gram–Schmidt QR decomposition. Returns Q. To obtain R, do R = QA
template <typename T>
std::pair<dense_matrix<T>, dense_matrix<T>> qr_decompose(dense_matrix<T> const& A) {
  assert(A.nrows() == A.ncols()); // A must be square

  const std::size_t N = A.nrows();

  std::vector<basic_vector<T>> basis{};
  basis.reserve(N);

  for (auto c : A.cols()) {
    basic_vector<T> a(c.begin(), c.end());
    auto u = std::transform_reduce(std::execution::unseq, basis.begin(), basis.end(), a,
      std::plus<basic_vector<T>>{}, [&a](auto const& e) { return -inner(e, a) * e; });
    basis.push_back(u.normalized());
  }

  dense_matrix<T> Q(N, N);
  dense_matrix<T> R(N, N);

  for (std::size_t r = 0; r != N; ++r) {
    for (std::size_t c = 0; c != N; ++c) {
      Q[r][c] = basis[c][r];
      R[r][c] = inner(basis[r], A.col(c));
    }
  }

  return {Q, R};
}

// Solve system Ax=b, where A is an upper triangular matrix
template <typename T>
basic_vector<T> triangularSolve(dense_matrix<T> const& A, basic_vector<T> const& b) {
  assert(A.nrows() == A.ncols()); // A must be square
  assert(A.nrows() == b.size());  // b must be the same size as A
  const std::size_t N = A.nrows();

  xmas::basic_vector<T> u(N);
  for (std::size_t i = N; i > 0; --i) {
    auto j = i - 1;
    u[j] = (b[j] - inner(A.row(j).drop(j), make_view(u).drop(j))) / A[j][j];
  }

  return u;
}

// Solve system Ax=b with QR decomposition
template <typename T>
basic_vector<T> QRsolve(is_matrix<T> auto const& A, basic_vector<T> const& b) {
  assert(A.nrows() == A.ncols()); // A must be square
  assert(A.nrows() == b.size());  // b must be the same size as A
  auto [Q, R] = xmas::algebra::qr_decompose(A);
  basic_vector<T> y = Q.transposed() * b;
  return triangularSolve(R, y);
}

// Solve system Ax=b with Gaussian elimination
template <typename T>
basic_vector<T> GaussSolve(dense_matrix<T> A, basic_vector<T> b) {
  assert(A.nrows() == A.ncols()); // A must be square
  assert(A.nrows() == b.size());  // b must be the same size as A
  const std::size_t N = A.nrows();

  constexpr T epsylon{1e-8};

  for (std::size_t i = 0; i < N - 1; ++i) {
    for (std::size_t k = i + 1; k < N; ++k) {
      if (std::abs(A[k][i]) < epsylon) {
        continue;
      }

      for (std::size_t j = i + 1; j < N; ++j) {
        A[k][j] = A[i][i] * A[k][j] - A[k][i] * A[i][j];
      }
      b[k] = A[i][i] * b[k] - A[k][i] * b[i];

      A[k][i] = T{0};

      const T q = A[k][i + 1];
      if (std::abs(q) < epsylon) {
        continue;
      }

      for (std::size_t j = i + 1; j < N; ++j) {
        A[k][j] /= q;
      }
      b[k] /= q;
    }
  }

  return triangularSolve(A, b);
}

// Solve system Ax=b with the conjugate gradients method.
template <typename T>
bool CGsolve(is_matrix<T> auto const& A, xmas::basic_vector<T> const& b, xmas::basic_vector<T>& x,
  T epsilon = 1e-8) {

  auto r = b - A * x;
  T err = r.norm2();
  if (err < epsilon) {
    return true;
  }

  auto p = r;
  for (std::size_t k = 0; k < x.size(); ++k) {
    T alpha = err / xmas::algebra::inner(p, A * p);
    x += alpha * p;
    r -= alpha * (A * p);

    T new_err = r.norm2();
    if (new_err < epsilon) {
      return true;
    }

    T beta = new_err / err;
    p = r + beta * p;

    err = new_err;
  }

  return false;
}

}
}
