#include "day09.hpp"

#include "xmaslib/functional/functional.hpp"
#include "xmaslib/log/log.hpp"
#include "xmaslib/line_iterator/line_iterator.hpp"
#include "xmaslib/parsing/parsing.hpp"

#include <algorithm>
#include <execution>
#include <functional>
#include <numeric>
#include <vector>

namespace {

std::int64_t extend(std::span<std::int64_t> const& values) {
  std::vector<std::int64_t> diffs(values.size() - 1);
  std::transform(std::execution::unseq, values.begin() + 1, values.end(), values.begin(),
    diffs.begin(), std::minus<std::int64_t>{});

  bool same_diff = std::all_of(
    std::execution::unseq, diffs.begin(), diffs.end(), xmas::equals<std::int64_t>(diffs.back()));
  if (same_diff) {
    return values.back() + diffs.back();
  }
  return values.back() + extend(diffs);
}

}

std::uint64_t Day09::part1() {
  xmas::views::linewise lines(this->input);

  std::vector<std::vector<std::int64_t>> data;
  std::transform(
    lines.begin(), lines.end(), std::back_inserter(data), xmas::parse_ints<std::int64_t>);

  std::int64_t n = std::transform_reduce(
    data.begin(), data.end(), std::int64_t{0}, std::plus<std::int64_t>{}, extend);

  if (n < 0) {
    xlog::warning("Solution is negative: {}", n);
    return static_cast<std::uint64_t>(-n);
  }
  return static_cast<std::uint64_t>(n);
}

std::uint64_t Day09::part2() {
  throw std::runtime_error("Not implemented");
}
