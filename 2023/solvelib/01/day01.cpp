#include "day01.hpp"
#include <algorithm>
#include <format>
#include <iostream>
#include <iterator>
#include <numeric>
#include <sstream>

bool is_num(char ch) { return ch >= '0' && ch <= '9'; }

std::int64_t Day01::part1() {
  std::stringstream ss(this->input);

  return std::transform_reduce(
      std::istream_iterator<std::string>(ss),
      std::istream_iterator<std::string>{}, 0, std::plus<int>{},
      [](std::string const line) {
        // First digit
        const auto first = std::ranges::find_if(line, is_num);
        if (first == line.end())
          throw std::runtime_error(
              std::format("line {} contains no numbers", line));

        // Last digit
        auto last = std::find_if(line.rbegin(), std::reverse_iterator(first), is_num);

        // compute
        return int(*first - '0') * 10 + int(*last - '0');
      });
}

std::int64_t Day01::part2() { return 2; }
