#include "day01.hpp"
#include <algorithm>
#include <format>
#include <iterator>
#include <numeric>
#include <sstream>
#include <utility>

bool is_num(char ch) { return ch >= '0' && ch <= '9'; }

std::uint64_t Day01::part1() {
  std::stringstream ss(this->input);

  return std::transform_reduce(
      std::istream_iterator<std::string>(ss),
      std::istream_iterator<std::string>{}, 0u, std::plus{},
      [](std::string_view line) -> std::uint64_t{
        // First digit
        const auto first = std::ranges::find_if(line, is_num);
        if (first == line.end()) {
          throw std::runtime_error(
              std::format("line {} contains no numbers", line));
        }

        // Last digit
        const auto last =
            std::find_if(line.rbegin(), std::reverse_iterator(first), is_num);

        // compute
        return static_cast<std::uint64_t>(int(*first - '0') * 10 + int(*last - '0'));
      });
}

template <typename Iterator>
std::pair<Iterator, int> find_number(Iterator begin, Iterator end) {
  constexpr static std::array numbers{"one", "two",   "three", "four", "five",
                                      "six", "seven", "eight", "nine"};

  if (begin == end) {
    return std::make_pair(end, 0);
  }

  auto inc = 1;
  Iterator endtext = end;
  if (begin > end) {
    // reverse
    inc = -1;
    endtext = begin;
  }

  for (Iterator it=begin; it != end; it += inc) {
    if (is_num(*it)) {
      return std::make_pair(it, int(*it - '0'));
    }

    std::string_view window(it, endtext);

    auto r = std::ranges::find_if(
        numbers, [window](auto number) { return window.starts_with(number); });
    if (r == numbers.end()) {
      continue;
    }

    return std::make_pair(it, r - numbers.begin() + 1);
  }

  return std::make_pair(end, 0);
}

std::uint64_t Day01::part2() {
  std::stringstream ss(this->input);

  return std::transform_reduce(
      std::istream_iterator<std::string>(ss),
      std::istream_iterator<std::string>{}, 0u, std::plus<std::uint64_t>{},
      [](std::string_view line) {
        // First digit
        const auto first = find_number(line.begin(), line.end());
        if (first.first == line.end())
          throw std::runtime_error(
              std::format("line {} contains no numbers", line));

        // Last digit
        const auto last = find_number(line.end(), first.first-1);

        // compute
        return first.second * 10 + last.second;
      });
}
