#include "solution.hpp"

#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace xmas {

void solution::run() {
  std::cout << "Day 1" << std::endl;
  try {
    const auto p1 = this->part1();
    std::cout << " Result 1: " << p1 << '\n';
  } catch (std::runtime_error &err) {
    std::cerr << "Part 1 failed with message: " << err.what() << std::endl;
  }

  try {
    const auto p2 = this->part2();
    std::cout << " Result 2: " << p2 << '\n';
  } catch (std::runtime_error &err) {
    std::cerr << "Part 2 failed with message: " << err.what() << std::endl;
  }
}

void solution::load(std::string_view path) {
  std::ifstream f(path.data());
  std::stringstream buff;
  buff << f.rdbuf();
  input = std::move(buff).str();

  if (input.size() == 0) {
    throw std::runtime_error(
        std::format("did not load any data from {}", path));
  }
}

} // namespace xmas