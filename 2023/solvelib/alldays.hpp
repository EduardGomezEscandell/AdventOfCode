#pragma once

#include "xmaslib/registry/registry.hpp"

#include "01/day01.hpp"
#include "02/day02.hpp"
#include "03/day03.hpp"
#include "04/day04.hpp"
#include "05/day05.hpp"
#include "11/day11.hpp"
#include "12/day12.hpp"
#include "13/day13.hpp"
#include "15/day15.hpp"

inline void populate_registry() {
  xmas::register_solution<Day01>();
  xmas::register_solution<Day02>();
  xmas::register_solution<Day03>();
  xmas::register_solution<Day04>();
  xmas::register_solution<Day05>();

  xmas::register_solution<Day11>();
  xmas::register_solution<Day12>();
  xmas::register_solution<Day13>();
  
  xmas::register_solution<Day15>();
}
