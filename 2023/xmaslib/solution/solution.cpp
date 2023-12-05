#include "solution.hpp"

#include <chrono>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "../log/log.hpp"

namespace xmas {

solution::duration solution::time() const {
  return this->time_p1 + this->time_p2;
}

bool solution::run(bool verbose) noexcept {
  bool success = true;

  if (verbose)
    xlog::info("Day {}", this->day());

  try {
    this->load();
  } catch (std::runtime_error &err) {
    xlog::error("Load failed with message: {}", err.what());
    return false;
  } catch (...) {
    xlog::error("Load failed with no message");
    return false;
  }

  try {
    const auto start = std::chrono::high_resolution_clock::now();
    const auto result = this->part1();
    this->time_p1 = std::chrono::high_resolution_clock::now() - start;

    if (verbose) {
      xlog::info(
          "result 1: {} ({} μs)", result,
          std::chrono::duration_cast<std::chrono::microseconds>(this->time_p1)
              .count());
    }
  } catch (std::runtime_error &err) {
    xlog::error("Part 1 failed with message: {}", err.what());
    success = false;
  } catch (...) {
    xlog::error("Part 1 failed with no message");
    return false;
  }

  try {
    const auto start = std::chrono::high_resolution_clock::now();
    const auto result = this->part2();
    this->time_p2 = std::chrono::high_resolution_clock::now() - start;

    if (verbose) {
      xlog::info(
          "Result 2: {} ({} μs)", result,
          std::chrono::duration_cast<std::chrono::microseconds>(this->time_p2)
              .count());
    }
  } catch (std::runtime_error &err) {
    xlog::error("Part 2 failed with message: {}", err.what());
    success = false;
  } catch (...) {
    xlog::error("Part 2 failed with no message");
    return false;
  }

  return success;
}

void solution::set_input(std::string_view path) { this->data_path = path; }

void solution::load() {
  std::ifstream f(this->data_path.data());
  if (!f) {
    throw std::runtime_error(
        std::format("could not open file {}", this->data_path));
  }

  std::stringstream buff;
  buff << f.rdbuf();
  input = std::move(buff).str();

  if (input.size() == 0) {
    xlog::warning("did not load any data from {}", this->data_path);
  }
}

} // namespace xmas