#include "solution.hpp"

#include <chrono>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "../log/log.hpp"

namespace xmas {

void solution::run() {
  xlog::info("Day {}", this->day());

  try {
    const auto start = std::chrono::high_resolution_clock::now();
    const auto result = this->part1();
    const auto elapsed = std::chrono::high_resolution_clock::now() - start;

    xlog::info(
        " Result 1: {} ({} μs)", result,
        std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count());
  } catch (std::runtime_error &err) {
    xlog::error("Part 1 failed with message: {}", err.what());
  }

  try {
    const auto start = std::chrono::high_resolution_clock::now();
    const auto result = this->part2();
    const auto elapsed = std::chrono::high_resolution_clock::now() - start;

    xlog::info(
        " Result 2: {} ({} μs)", result,
        std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count());
  } catch (std::runtime_error &err) {
    xlog::error("Part 2 failed with message: {}", err.what());
  }
}

void solution::load(std::string_view path) {
  std::ifstream f(path.data());
  if (!f) {
    throw std::runtime_error(std::format("could not open file {}", path));
  }

  std::stringstream buff;
  buff << f.rdbuf();
  input = std::move(buff).str();

  if (input.size() == 0) {
    xlog::warning("did not load any data from {}", path);
  }
}

} // namespace xmas