#pragma once

#include <cstdint>
#include <iostream>
#include <optional>
#include <stdexcept>

namespace xmas {

class solution {
public:
  virtual int day() = 0;

  virtual void run()  {
    std::cout << "Day 1" << std::endl;
    try {
      const auto p1 =this->part1();
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

protected:
  virtual std::int64_t part1() { throw std::runtime_error("not implemented"); }
  virtual std::int64_t part2() { throw std::runtime_error("not implemented"); }

private:
  std::optional<std::int64_t> p1;
  std::optional<std::int64_t> p2;
};

} // namespace xmas