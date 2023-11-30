#pragma once

#include <vector>
#include <string_view>
#include <map>
#include <memory>

#include "xmaslib/solvebase/solvebase.hpp"


int run(std::vector<std::string_view> &args);

void usage(std::ostream &s);

bool solve_day(
    std::map<const int, std::unique_ptr<xmas::solution>>::value_type const
        &solution);