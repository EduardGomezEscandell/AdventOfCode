#pragma once

#include "app.hpp"
#include "xmaslib/solution/solution.hpp"

#include <map>
#include <memory>
#include <vector>

namespace app {

using solution_vector = std::vector<
    std::map<const int, std::unique_ptr<xmas::solution>>::const_iterator>;

solution_vector select_days(argv days);
solution_vector select_all_days();
bool execute_days(app &, solution_vector const &days);

} // namespace app