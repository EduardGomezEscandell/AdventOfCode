#pragma once

#include <cstdlib>
#include <functional>
#include <map>
#include <span>
#include <string>
#include <string_view>
#include <vector>

constexpr int exit_success = EXIT_SUCCESS;
constexpr int exit_failure = EXIT_FAILURE;
constexpr int exit_bad_args = 2;

namespace app {
class app;
using argv = std::span<std::string_view>;

struct command {
  std::vector<std::string_view> flags;
  std::string_view help;
  std::function<int(app&, argv)> run = [](app&, argv) { return exit_success; };
};

class app {
public:
  void register_command(command const&);
  std::string usage();
  int run(argv args);

private:
  struct wrapped_command {
    command c;
    argv args;
  };

  std::map<std::string_view, command> commands;
  std::vector<command> commands_flat;
};

} // namespace app