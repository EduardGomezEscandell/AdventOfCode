#include "xmaslib/log/log.hpp"

#include "app.hpp"
#include "cmd.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string_view>
#include <vector>

int main(int argc, char **argv) {
  std::vector<std::string_view> args;
  args.reserve(static_cast<std::size_t>(argc));
  for (auto i = 1; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }

  app::app a;

  a.register_command({
      .flags = {"-h", "--help"},
      .help = "Shows this message and exits",
      .run = [](app::app &app, app::argv args) -> int {
        if (args.size() != 0) {
          xlog::error("help takes no arguments");
          return exit_bad_args;
        }
        std::cout << app.usage();
        return exit_success;
      },
  });

  a.register_command({
      .flags = {"-r", "--run"},
      .help = "Run the solutions for the specified days",
      .run =
          [](app::app &app, app::argv args) {
            auto selection = app::select_days(args);
            if (selection.size() == 0) {
              xlog::error("No days selected");
              return exit_bad_args;
            }

            const bool success = app::execute_days(app, selection);
            if (success) {
              return exit_success;
            }
            return exit_failure;
          },
  });

  a.register_command({
      .flags = {"-a", "--all"},
      .help = "Run all the solutions",
      .run =
          [](app::app &app, app::argv args) {
            if (args.size() != 0) {
              xlog::error("--all takes no arguments");
              return exit_bad_args;
            }

            auto selection = app::select_all_days();
            if (selection.size() == 0) {
              xlog::error("No days available");
              return exit_failure;
            }

            const bool success = app::execute_days(app, selection);
            if (success) {
              return exit_success;
            }
            return exit_failure;
          },
  });

  a.register_command({
      .flags = {"-t", "--time"},
      .help = "Runs all the solutions many times to get an accurate profile",
      .run =
          [](app::app &app, app::argv args) {
            if (args.size() != 0) {
              xlog::error("--time takes no arguments");
              return exit_bad_args;
            }

            auto selection = app::select_all_days();
            if (selection.size() == 0) {
              xlog::error("No days available");
              return exit_failure;
            }

            const bool success = app::time_days(app, selection, std::chrono::seconds(5));
            if (success) {
              return exit_success;
            }
            return exit_failure;
          },
  });

  return a.run(args);
}
