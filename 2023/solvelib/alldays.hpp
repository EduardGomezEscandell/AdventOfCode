#pragma once

#include "xmaslib/registry/registry.hpp"

#include "01/day01.hpp"

inline void populate_registry() {
    xmas::register_solution<1, Day01>();
}