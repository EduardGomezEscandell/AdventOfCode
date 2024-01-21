#pragma once

#include <doctest/doctest.h>

#include "dense_algebra.hpp"
#include "dense_matrix.hpp"
#include "../log/log.hpp"

TEST_CASE("QR decomoposion") {

  SUBCASE("Wikipedia example") {
    xmas::dense_matrix<float> A(3, 3);

    // clang-format off
    A[0][0] = 12;     A[0][1] = -51;  A[0][2] =   4;
    A[1][0] =  6;     A[1][1] = 167;  A[1][2] = -68;
    A[2][0] = -4;     A[2][1] =  24;  A[2][2] = -41;
    // clang-format on

    auto [Q, _] = xmas::algebra::qr_decompose(A);

    xlog::debug("Q = \n{}", Q.format("{:>5}"));

    CHECK_EQ(Q[0][0], doctest::Approx(6 / 7.0f));
    CHECK_EQ(Q[0][1], doctest::Approx(-69 / 175.0f));
    CHECK_EQ(Q[0][2], doctest::Approx(-58 / 175.0f));

    CHECK_EQ(Q[1][0], doctest::Approx(3 / 7.0f));
    CHECK_EQ(Q[1][1], doctest::Approx(158 / 175.0f));
    CHECK_EQ(Q[1][2], doctest::Approx(6 / 175.0f));

    CHECK_EQ(Q[2][0], doctest::Approx(-2 / 7.0f));
    CHECK_EQ(Q[2][1], doctest::Approx(6 / 35.0f));
    CHECK_EQ(Q[2][2], doctest::Approx(-33 / 35.0f));
  }

  SUBCASE("Example") {
    xmas::dense_matrix<float> A(3, 3);

    // clang-format off
    A[0][0] = 4;     A[0][1] = 2;  A[0][2] = 1;
    A[1][0] = 1;     A[1][1] = 1;  A[1][2] = 1;
    A[2][0] = 0;     A[2][1] = 0;  A[2][2] = 1;
    // clang-format on

    auto [Q, _] = xmas::algebra::qr_decompose(A);

    xlog::debug("Q = \n{}", Q.format("{:>5}"));

    CHECK_EQ(Q[0][0], doctest::Approx(0.970142500145332));
    CHECK_EQ(Q[0][1], doctest::Approx(-0.242535625036333));
    CHECK_EQ(Q[0][2], doctest::Approx(0.0));

    CHECK_EQ(Q[1][0], doctest::Approx(0.242535625036333));
    CHECK_EQ(Q[1][1], doctest::Approx(0.970142500145332));
    CHECK_EQ(Q[1][2], doctest::Approx(0.0));

    CHECK_EQ(Q[2][0], doctest::Approx(0.0));
    CHECK_EQ(Q[2][1], doctest::Approx(0.0));
    CHECK_EQ(Q[2][2], doctest::Approx(1.0));
  }

  SUBCASE("Example 2") {
    xmas::dense_matrix<float> A(3, 3);

    // clang-format off
    A[0][0] = 0;     A[0][1] = 0;  A[0][2] = 1;
    A[1][0] = 1;     A[1][1] = 1;  A[1][2] = 1;
    A[2][0] = 4;     A[2][1] = 2;  A[2][2] = 1;
    // clang-format on

    auto [Q, R] = xmas::algebra::qr_decompose(A);

    xlog::debug("Q = \n{}", Q.format("{:>5}"));
    xlog::debug("R = \n{}", R.format("{:>5}"));

    // Q

    CHECK_EQ(Q[0][0], doctest::Approx(0.0));
    CHECK_EQ(Q[0][1], doctest::Approx(0.0));
    CHECK_EQ(Q[0][2], doctest::Approx(1.0));

    CHECK_EQ(Q[1][0], doctest::Approx(0.242535625036333));
    CHECK_EQ(Q[1][1], doctest::Approx(0.970142500145332));
    CHECK_EQ(Q[1][2], doctest::Approx(0.0));

    CHECK_EQ(Q[2][0], doctest::Approx(0.970142500145332));
    CHECK_EQ(Q[2][1], doctest::Approx(-0.2425356250363330));
    CHECK_EQ(Q[2][2], doctest::Approx(0.0));

    // R

    CHECK_EQ(R[0][0], doctest::Approx(4.123105625617661));
    CHECK_EQ(R[0][1], doctest::Approx(2.182820625326997));
    CHECK_EQ(R[0][2], doctest::Approx(1.212678125181665));

    CHECK_EQ(R[1][0], doctest::Approx(0.0));
    CHECK_EQ(R[1][1], doctest::Approx(0.485071250072666));
    CHECK_EQ(R[1][2], doctest::Approx(0.7276068751089988));

    CHECK_EQ(R[2][0], doctest::Approx(0.0));
    CHECK_EQ(R[2][1], doctest::Approx(0.0));
    CHECK_EQ(R[2][2], doctest::Approx(1.0));
  }
}
