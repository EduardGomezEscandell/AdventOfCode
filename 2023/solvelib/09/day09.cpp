#include "day09.hpp"

#include "xmaslib/functional/functional.hpp"
#include "xmaslib/log/log.hpp"
#include "xmaslib/line_iterator/line_iterator.hpp"
#include "xmaslib/parsing/parsing.hpp"

#include <algorithm>
#include <execution>
#include <functional>
#include <numeric>
#include <string_view>
#include <vector>

namespace {

template <bool reverse>
std::int64_t extend(std::span<std::int64_t> values) {
  std::vector<std::int64_t> diffs(values.size() - 1);
  std::transform(std::execution::unseq, values.begin() + 1, values.end(), values.begin(),
    diffs.begin(), std::minus<std::int64_t>{});

  bool same_diff = std::all_of(
    std::execution::unseq, diffs.begin(), diffs.end(), xmas::equals<std::int64_t>(diffs.back()));

  if (reverse) {
    if (same_diff) {
      return values.front() - diffs.front();
    }
    return values.front() - extend<reverse>(diffs);
  }

  if (same_diff) {
    return values.back() + diffs.back();
  }
  return values.back() + extend<reverse>(diffs);
}

template <bool reverse>
std::uint64_t solve(std::string_view input) {
  xmas::views::linewise lines(input);

  std::vector<std::vector<std::int64_t>> data;
  std::transform(
    lines.begin(), lines.end(), std::back_inserter(data), xmas::parse_ints<std::int64_t>);

  std::int64_t n = std::transform_reduce(
    data.begin(), data.end(), std::int64_t{0}, std::plus<std::int64_t>{}, extend<reverse>);

  if (n < 0) {
    xlog::warning("Solution is negative: {}", n);
    return static_cast<std::uint64_t>(-n);
  }
  return static_cast<std::uint64_t>(n);
}

}

std::uint64_t Day09::part1() {
  return solve<false>(this->input);
}

std::uint64_t Day09::part2() {
  return solve<true>(this->input);
}
