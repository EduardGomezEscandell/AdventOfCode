#include "app.hpp"

#include "xmaslib/log/log.hpp"

#include <algorithm>
#include <format>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string_view>

namespace app {

std::string replace(std::string_view in, char target, std::string_view replacement) {
  std::string out;
  out.reserve(in.size());

  for (auto ch : in) {
    if (ch != target) {
      out.push_back(ch);
      continue;
    }

    out += replacement;
  }

  return out;
}

std::string app::usage() {
  std::stringstream s;
  s << "Usage:\n";

  for (auto cmd : commands_flat) {
    s << '\n';
    for (auto& alias : cmd.flags) {
      s << std::format("aoc2023 {}\n", alias);
    }

    constexpr auto prefix = "    ";
    std::string help = prefix + replace(cmd.help, '\n', std::format("\n{}", prefix));
    s << help << '\n';
  }

  return s.str();
}

void app::register_command(command const& cmd) {
  this->commands_flat.push_back(cmd);

  for (auto alias : cmd.flags) {
    auto it = this->commands.find(alias);
    if (it != this->commands.end()) {
      throw std::runtime_error(std::format("Flag {} is already defined", alias));
    }

    this->commands[alias] = cmd;
  }
}

int app::run(argv args) {
  if (args.size() == 0) {
    this->usage();
    return exit_bad_args;
  }

  std::vector<wrapped_command> cmd;

  // Parse commands
  auto it = args.begin();
  while (it != args.end()) {
    auto c = *it;

    auto icmd = commands.find(c);
    if (icmd == commands.end()) {
      xlog::error("Unknown command {}", c);
      return exit_bad_args;
    }

    auto argv_begin = it + 1;
    auto argv_end =
      std::find_if(argv_begin, args.end(), [](std::string_view s) { return s.starts_with("-"); });

    xlog::debug("Found command {} with {} arguments", c, argv_end - argv_begin);
    cmd.emplace_back(icmd->second, std::span{argv_begin, argv_end});

    it = argv_end;
  }

  // Execute commands
  for (auto& command : cmd) {
    auto r = command.c.run(*this, command.args);
    switch (r) {
    case exit_success:
      continue;
    case exit_bad_args:
      std::cerr << this->usage();
      return exit_bad_args;
    default:
      return r;
    }
  }

  return exit_success;
}

} // namespace app