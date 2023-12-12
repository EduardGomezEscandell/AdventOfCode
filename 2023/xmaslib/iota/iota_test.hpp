#include "iota.hpp"

#include <doctest/doctest.h>

#include <algorithm>
#include <cstdint>
#include <functional>
#include <iterator>
#include <numeric>
#include <vector>

TEST_CASE("iota iterator") {

  SUBCASE("Access") {
    xmas::views::iota<std::int8_t> iota(3, 10);
    REQUIRE_EQ(iota.size(), 7);

    CHECK_EQ(iota.at(0), 3);
    CHECK_EQ(iota.at(1), 4);
    CHECK_EQ(iota.at(2), 5);
    CHECK_EQ(iota.at(3), 6);
    CHECK_EQ(iota.at(4), 7);
    CHECK_EQ(iota.at(5), 8);
    CHECK_EQ(iota.at(6), 9);

    REQUIRE_THROWS(iota.at(7));

    CHECK_EQ(iota[0], 3);
    CHECK_EQ(iota[1], 4);
    CHECK_EQ(iota[2], 5);
    CHECK_EQ(iota[3], 6);
    CHECK_EQ(iota[4], 7);
    CHECK_EQ(iota[5], 8);
    CHECK_EQ(iota[6], 9);
  }

  SUBCASE("Copy") {
    xmas::views::iota<std::size_t> iota(11, 17);
    std::vector<std::size_t> out;

    std::copy(iota.cbegin(), iota.cend(), std::back_inserter(out));

    REQUIRE_EQ(iota.size(), 6);
    REQUIRE_EQ(out.size(), 6);

    CHECK_EQ(out[0], 11);
    CHECK_EQ(out[1], 12);
    CHECK_EQ(out[2], 13);
    CHECK_EQ(out[3], 14);
    CHECK_EQ(out[4], 15);
    CHECK_EQ(out[5], 16);
  }

  SUBCASE("Reduce") {
    xmas::views::iota<int> iota(0, 10);
    REQUIRE_EQ(iota.size(), 10u);

    auto got = std::reduce(iota.begin(), iota.end(), 0, std::plus<int>{});
    REQUIRE_EQ(got, 45);
  }
}