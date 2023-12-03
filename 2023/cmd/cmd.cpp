#include "cmd.hpp"
#include "solvelib/alldays.hpp"
#include "xmaslib/registry/registry.hpp"
#include "xmaslib/log/log.hpp"

#include <algorithm>
#include <cstdlib>
#include <format>
#include <functional>
#include <iostream>
#include <numeric>
#include <ranges>
#include <stdexcept>
#include <string_view>
#include <vector>

constexpr int exit_success = EXIT_SUCCESS;
constexpr int exit_bad_args = 2;
constexpr int exit_error = 1;

int run(std::vector<std::string_view> &args) {
  if (args.size() == 0) {
    usage(std::cerr);
    return exit_bad_args;
  }

  if (args.size() == 1 && (args[0] == "-h" || args[0] == "--help")) {
    usage(std::cout);
    return exit_success;
  }

  try {
    populate_registry();
  } catch (std::runtime_error &e) {
    xlog::error("could not populate the registry fully: {}", e.what());
  }
  const auto &solutions = xmas::registered_solutions();

  // Run all solutions
  if (args.size() == 1 && (args[0] == "-a" || args[0] == "--all")) {
    std::for_each(solutions.begin(), solutions.end(), solve_day);
    return exit_success;
  }

  if(args[0].starts_with("-")) {
    xlog::error("Unknown argument {}. Use --help to see possible inputs.", args[0]);
    return exit_bad_args;
  }

  // Parse days requested
  std::vector<std::map<const int, std::unique_ptr<xmas::solution>>::const_iterator>
      days;
  days.reserve(args.size());
  for (auto &arg : args) {
    if (arg.starts_with("-")) {
      xlog::error("invalid mixed argument types, use --help to see valid inputs");
      return exit_bad_args;
    }

    const int day = std::atoi(arg.data());
    if (day == 0) {
      xlog::warning("{} is not a vaid day", day);
      continue;
    }

    auto it = solutions.find(day);
    if (it == solutions.end()) {
      xlog::warning("no solution registered for day {}", day);
      continue;
    }

    days.push_back(it);
  }

  // No critical error: execute
  const auto results = days | std::ranges::views::transform(
                                  [](auto it) { return solve_day(*it); });
  const bool all_successful =
      std::reduce(results.begin(), results.end(), true, std::logical_and{});

  if (!all_successful) {
    return exit_error;
  }

  return exit_success;
}

void usage(std::ostream &s) {
  s << "Usage:\n\n"
    << "aoc2023 -h\n"
    << "aoc2023 --help\n"
    << "   Prints this message and exits\n\n"
    << "aoc2023 DAYS...\n"
    << "   Runs the solution for the specified days\n\n"
    << "aoc2023 -a\n"
    << "aoc2023 --all\n"
    << "   Runs all solutions\n\n";
}

std::string datafile(int day) {
  return std::format("./data/{:02d}/input.txt", day);
}

bool solve_day(
    std::map<const int, std::unique_ptr<xmas::solution>>::value_type const
        &solution) {
  
  try {
    solution.second.get()->load(datafile(solution.second->day()));
  } catch (std::runtime_error &e) {
    xlog::error("day {} could not load: {}\n", solution.second.get()->day(),
              e.what());
    return false;
  }
  
  
  try {
    solution.second.get()->run();
    return true;
  } catch (std::runtime_error &e) {
    xlog::error("day {} returned error: {}\n", solution.second.get()->day(),
              e.what());
    return false;
  }
}
