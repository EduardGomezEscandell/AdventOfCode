#pragma once

#include <cstdlib>
#include <map>
#include <string_view>
#include <vector>

constexpr int exit_success = EXIT_SUCCESS;
constexpr int exit_failure = EXIT_FAILURE;
constexpr int exit_bad_args = 2;

class app {
public:
  using argv = std::vector<std::string_view>;
  using mainfn = int (*)(app &, argv const &);

  app();

  int run(std::vector<std::string_view> &args);

  std::map<std::string_view, mainfn> commands;

private:
  static int help(app &, argv const &days);
  static int exec_some_days(app &, argv const &days);
  static int exec_all_days(app &, argv const &days);

  bool any_action;
};
