#include <doctest/doctest.h>

#include "lru.hpp"

TEST_CASE("LRU cache") {
  xmas::lru_cache<int, int> cache(3);

  REQUIRE_FALSE(cache.get(15).has_value());

  cache.set(1, 1000);

  auto opt = cache.get(1);
  REQUIRE(opt.has_value());
  REQUIRE_EQ(*opt, 1000);

  cache.set(2, 2000);

  opt = cache.get(1);
  REQUIRE(opt.has_value());
  REQUIRE_EQ(*opt, 1000);
  opt = cache.get(2);
  REQUIRE(opt.has_value());
  REQUIRE_EQ(*opt, 2000);

  cache.set(3, 3000);

  opt = cache.get(1);
  REQUIRE(opt.has_value());
  REQUIRE_EQ(*opt, 1000);
  opt = cache.get(2);
  REQUIRE(opt.has_value());
  REQUIRE_EQ(*opt, 2000);
  opt = cache.get(3);
  REQUIRE(opt.has_value());
  REQUIRE_EQ(*opt, 3000);

  // Pushing it beyond its max capacity

  cache.set(4, 4000);

  // Old entry no loner exists
  opt = cache.get(1);
  REQUIRE_FALSE(opt.has_value());

  opt = cache.get(2);
  REQUIRE(opt.has_value());
  REQUIRE_EQ(*opt, 2000);
  opt = cache.get(3);
  REQUIRE(opt.has_value());
  REQUIRE_EQ(*opt, 3000);
  opt = cache.get(4);
  REQUIRE(opt.has_value());
  REQUIRE_EQ(*opt, 4000);

  // Override a key
  cache.set(3, 3500);
  opt = cache.get(3);
  REQUIRE(opt.has_value());
  REQUIRE_EQ(*opt, 3500);
}