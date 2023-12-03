#include "solution.hpp"

#include <chrono>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "../log/log.hpp"

namespace xmas {

bool solution::run() noexcept {
  xlog::info("Day {}", this->day());

  try {
  this->load();
  } catch (std::runtime_error &err) {
    xlog::error("Load failed with message: {}", err.what());
    return false;
  } catch(...) {
    xlog::error("Load failed with no message");
    return false;
  }

  try {
    const auto start = std::chrono::high_resolution_clock::now();
    const auto result = this->part1();
    const auto elapsed = std::chrono::high_resolution_clock::now() - start;

    xlog::info(
        "Result 1: {} ({} μs)", result,
        std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count());
  } catch (std::runtime_error &err) {
    xlog::error("Part 1 failed with message: {}", err.what());
  }  catch(...) {
    xlog::error("Part 1 failed with no message");
    return false;
  }

  try {
    const auto start = std::chrono::high_resolution_clock::now();
    const auto result = this->part2();
    const auto elapsed = std::chrono::high_resolution_clock::now() - start;

    xlog::info(
        "Result 2: {} ({} μs)", result,
        std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count());
  } catch (std::runtime_error &err) {
    xlog::error("Part 2 failed with message: {}", err.what());
  } catch(...) {
    xlog::error("Part 2 failed with no message");
    return false;
  }

  return true;
}

void solution::set_input(std::string_view path) { this->data_path = path; }

void solution::load() {
  std::ifstream f(this->data_path.data());
  if (!f) {
    throw std::runtime_error(std::format("could not open file {}", this->data_path));
  }

  std::stringstream buff;
  buff << f.rdbuf();
  input = std::move(buff).str();

  if (input.size() == 0) {
    xlog::warning("did not load any data from {}", this->data_path);
  }
}

} // namespace xmas