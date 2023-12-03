#pragma once

#include "xmaslib/registry/registry.hpp"

#include "01/day01.hpp"
#include "02/day02.hpp"

inline void populate_registry() {
  xmas::register_solution<Day01>();
  xmas::register_solution<Day02>();
}
