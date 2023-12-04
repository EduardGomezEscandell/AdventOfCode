#include "cmd.hpp"
#include "solvelib/alldays.hpp"
#include "xmaslib/log/log.hpp"
#include "xmaslib/solution/solution.hpp"

#include <charconv>
#include <memory>
#include <stdexcept>
#include <string_view>

namespace app {

std::optional<xmas::solution::duration>
solve_day(std::map<const int, std::unique_ptr<xmas::solution>>::value_type const
              &solution);

solution_vector select_all_days() {
  try {
    populate_registry();
  } catch (std::runtime_error &e) {
    xlog::error("could not populate the registry fully: {}", e.what());
  }
  const auto &solutions = xmas::registered_solutions();

  solution_vector days;

  for (auto it = solutions.cbegin(); it != solutions.cend(); ++it) {
    days.push_back(it);
  }

  return days;
}

solution_vector select_days(argv days) {
  try {
    populate_registry();
  } catch (std::runtime_error &e) {
    xlog::error("could not populate the registry fully: {}", e.what());
  }
  const auto &solutions = xmas::registered_solutions();

  solution_vector out;

  out.reserve(days.size());
  for (auto &day : days) {
    int d = -1;
    std::from_chars(day.begin(), day.end(), d);
    if (d == -1) {
      xlog::warning("Value {} is not a valid day", day);
      continue;
    }

    auto it = solutions.find(d);
    if (it == solutions.end()) {
      xlog::warning("No solution registered for day {}", day);
      continue;
    }

    out.push_back(it);
  }

  return out;
}

bool execute_days(app &, solution_vector const &days) {
  xmas::solution::duration total{};
  bool total_success = true;

  for (auto d : days) {
    const auto t = solve_day(*d);
    if (!t.has_value()) {
      total_success = false;
      continue;
    }

    total += *t;
  }

  xlog::info("DONE");
  xlog::info(
      "Total time was {} ms",
      std::chrono::duration_cast<std::chrono::milliseconds>(total).count());

  return total_success;
}

std::optional<xmas::solution::duration>
solve_day(std::map<const int, std::unique_ptr<xmas::solution>>::value_type const
              &solution) {

  try {
    solution.second->set_input(
        std::format("./data/{:02d}/input.txt", solution.second->day()));
  } catch (std::runtime_error &e) {
    xlog::error("day {} could not load: {}\n", solution.second->day(),
                e.what());
    return {};
  }

  if (auto success = solution.second->run(); !success) {
    return {};
  }

  return {solution.second->time()};
}

} // namespace app