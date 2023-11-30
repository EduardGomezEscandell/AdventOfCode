// #include "xmaslib/solvebase/solvebase.hpp"
#include "cmd.hpp"

#include <cstdlib>
#include <string_view>
#include <vector>



int main(int argc, char **argv) {
  std::vector<std::string_view> args;
  args.reserve(static_cast<std::size_t>(argc));
  for (auto i = 1; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }

  return run(args);
}
