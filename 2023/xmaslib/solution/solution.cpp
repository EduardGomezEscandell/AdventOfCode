#include "solution.hpp"

#include <chrono>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <unistd.h>

namespace xmas {

void solution::run() {
  std::cout << "Day 1" << std::endl;
  
  try {
    const auto start = std::chrono::high_resolution_clock::now();
    const auto result = this->part1();
    const auto elapsed = std::chrono::high_resolution_clock::now() - start;
    
    std::cout << std::format(" Result 1: {} ({} μs)\n", result, std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count());
  } catch (std::runtime_error &err) {
    std::cerr << "Part 1 failed with message: " << err.what() << std::endl;
  }

  try {
    const auto start = std::chrono::high_resolution_clock::now();
    const auto result = this->part2();
    const auto elapsed = std::chrono::high_resolution_clock::now() - start;
    
    std::cout << std::format(" Result 2: {} ({} μs)\n", result, std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count());
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