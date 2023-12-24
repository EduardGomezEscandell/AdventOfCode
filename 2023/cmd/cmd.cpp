#include "cmd.hpp"
#include "solvelib/alldays.hpp"
#include "xmaslib/log/log.hpp"
#include "xmaslib/solution/solution.hpp"

#include <charconv>
#include <chrono>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <string_view>

namespace app {

std::optional<xmas::solution::duration> solve_day(
  std::map<const int, std::unique_ptr<xmas::solution>>::value_type const& solution,
  bool verbose);

solution_vector select_all_days() {
  try {
    populate_registry();
  } catch (std::runtime_error& e) {
    xlog::error("could not populate the registry fully: {}", e.what());
  }
  const auto& solutions = xmas::registered_solutions();

  solution_vector days;

  for (auto it = solutions.cbegin(); it != solutions.cend(); ++it) {
    days.push_back(it);
  }

  return days;
}

solution_vector select_days(argv days) {
  try {
    populate_registry();
  } catch (std::runtime_error& e) {
    xlog::error("could not populate the registry fully: {}", e.what());
  }
  const auto& solutions = xmas::registered_solutions();

  solution_vector out;

  out.reserve(days.size());
  for (auto& day : days) {
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

bool execute_days(app&, solution_vector const& days) {
  xmas::solution::duration total{};
  bool total_success = true;

  for (auto d : days) {
    const auto t = solve_day(*d, true);
    if (!t.has_value()) {
      total_success = false;
      continue;
    }

    total += *t;
  }

  xlog::info("DONE");
  xlog::info(
    "Total time was {} ms", std::chrono::duration_cast<std::chrono::milliseconds>(total).count());

  return total_success;
}

std::int64_t microseconds(xmas::solution::duration d) {
  return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
}

bool time_days(app&, solution_vector const& days, xmas::solution::duration timeout) {
  xmas::solution::duration total{};
  bool total_success = true;

  xlog::debug("Timing solution in debug mode is not recommended");

  xlog::info("Every day's solution will be executed non-stop for {} seconds to "
             "evaluate its performance.",
    std::chrono::duration_cast<std::chrono::seconds>(timeout).count());

  constexpr std::string_view fmt = "{:>7} {:>8} {:>7}    {:>7}";
  xlog::info("");
  xlog::info("    DAY    COUNT  MEAN(μs)  DEVIATION(μs)");

  for (auto d : days) {
    auto begin = std::chrono::high_resolution_clock::now();

    std::int64_t iter = 0;
    xmas::solution::duration daily_total{};

    // Used to compute standard deviation
    std::int64_t M = 0, S = 0;

    // Disable warning and debug messages (They'll be spammed because
    // the soultion is re-run many times)
    xlog::logger::global().set_severity(xlog::ERROR);

    while (std::chrono::high_resolution_clock::now() - begin < timeout) {
      const auto t = solve_day(*d, false);
      if (!t.has_value()) {
        total_success = false;
        continue;
      }
      daily_total += *t;
      ++iter;

      // std deviation stuff
      // https://mathcentral.uregina.ca/QQ/database/QQ.09.02/carlos1.html
      auto us = microseconds(*t);
      const auto prevM = M;
      M += (us - prevM) / iter;
      S += (us - prevM) * (us - M);
    }

    xlog::logger::global().set_severity(xlog::DEBUG);

    if (iter == 0) {
      xlog::warning("Could not successfully complete day {}", d->second->day());
      continue;
    }

    // Average
    const auto mean =
      std::chrono::duration_cast<std::chrono::microseconds>(daily_total / iter).count();
    const auto dev = static_cast<std::int64_t>(std::sqrt(S / iter));

    // Report
    xlog::info(fmt, d->second->day(), iter, mean, dev);

    total += daily_total / iter;
  }

  xlog::info(
    fmt, "TOTAL", "-", std::chrono::duration_cast<std::chrono::microseconds>(total).count(), "-");

  return total_success;
}

std::optional<xmas::solution::duration> solve_day(
  std::map<const int, std::unique_ptr<xmas::solution>>::value_type const& solution,
  bool verbose) {

  try {
    solution.second->set_input(std::format("./data/{:02d}/input.txt", solution.second->day()));
  } catch (std::runtime_error& e) {
    xlog::error("day {} could not load: {}\n", solution.second->day(), e.what());
    return {};
  }

  if (auto success = solution.second->run(verbose); !success) {
    return {};
  }

  return {solution.second->time()};
}

} // namespace app