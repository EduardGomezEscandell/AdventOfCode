#include "cmd.hpp"
#include "solvelib/alldays.hpp"
#include "xmaslib/log/log.hpp"
#include "xmaslib/registry/registry.hpp"
#include "xmaslib/solution/solution.hpp"

#include <chrono>
#include <cstdlib>
#include <format>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <vector>

using solution_vector = std::vector<
    std::map<const int, std::unique_ptr<xmas::solution>>::const_iterator>;

void usage(std::ostream &s);
std::optional<xmas::solution::duration>
solve_day(std::map<const int, std::unique_ptr<xmas::solution>>::value_type const
              &solution);

app::app() {
  this->commands = {
      {"-h", help},
      {"--help", help},
      {"--all", exec_all_days},
      {"-a", exec_all_days},
  };
}

solution_vector all_days() {
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

solution_vector select_days(app::argv args) {
  try {
    populate_registry();
  } catch (std::runtime_error &e) {
    xlog::error("could not populate the registry fully: {}", e.what());
  }
  const auto &solutions = xmas::registered_solutions();

  solution_vector days;

  days.reserve(args.size());
  for (auto &arg : args) {
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

  return days;
}

bool execute(app &, solution_vector const &days) {
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

int app::help(app &a, app::argv const &days) {
  a.any_action = true;
  if (days.size() != 0) {
    xlog::error("option --help is incompatible with a list of days");
    return exit_bad_args;
  }
  usage(std::cout);
  return exit_success;
}

int app::exec_all_days(app &a, app::argv const &days) {
  if (days.size() != 0) {
    xlog::error("option --all is incompatible with a list of days");
    return exit_bad_args;
  }
  a.any_action = true;

  const bool success = execute(a, all_days());
  if (success) {
    return exit_success;
  }
  return exit_failure;
}

int app::exec_some_days(app &a, app::argv const &days) {
  if (days.size() == 0) {
    return exit_success;
  }

  auto selection = select_days(days);
  if (selection.size() == 0) {
    return exit_success;
  }

  a.any_action = true;

  const bool success = execute(a, selection);
  if (success) {
    return exit_success;
  }
  return exit_failure;
}

int app::run(argv &args) {
  if (args.size() == 0) {
    usage(std::cerr);
    return exit_bad_args;
  }

  argv day_args;
  argv cmd_args;

  for (auto arg : args) {
    if (arg.starts_with("-")) {
      cmd_args.push_back(arg);
      continue;
    }
    day_args.push_back(arg);
  }

  for (auto &cmd : cmd_args) {
    auto it = commands.find(cmd);
    if (it == commands.end()) {
      xlog::error("Unknown argument {}. Use --help to see possible inputs.",
                  args[0]);
      return exit_bad_args;
    }

    if (auto ret = it->second(*this, day_args); ret != exit_success) {
      return ret;
    }
  }

  if (auto ret = exec_some_days(*this, day_args); ret != exit_success) {
    return ret;
  }

  if (!this->any_action) {
    xlog::warning("No solutions executed");
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

std::optional<xmas::solution::duration>
solve_day(std::map<const int, std::unique_ptr<xmas::solution>>::value_type const
              &solution) {

  try {
    solution.second->set_input(datafile(solution.second->day()));
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
