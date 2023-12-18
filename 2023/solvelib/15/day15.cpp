#include "day15.hpp"

#include "xmaslib/iota/iota.hpp"
#include "xmaslib/parsing/parsing.hpp"

#include <algorithm>
#include <execution>
#include <functional>
#include <iterator>
#include <list>
#include <numeric>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

struct hash_map {
  using key_t = std::string;
  using value_t = std::uint64_t;

  using bucket_t = std::list<std::pair<key_t, value_t>>;

  std::array<bucket_t, 256> buckets;

  void insert(key_t &&k, value_t &&v) {
    auto &bucket = buckets[Day15::hash256(k)];
    auto it = std::ranges::find_if(
        bucket, [&k](auto const &pair) { return pair.first == k; });
    if (it == bucket.end()) {
      bucket.insert(
          it, std::make_pair(std::forward<key_t>(k), std::forward<value_t>(v)));
      return;
    }

    it->first = std::forward<key_t>(k);
    it->second = std::forward<value_t>(v);
  }

  void remove(key_t const &k) {
    auto &bucket = buckets[Day15::hash256(k)];
    auto it = std::ranges::find_if(bucket,
                                   [&k](auto const& pair) { return pair.first == k; });
    if (it == bucket.end()) {
      return; // Nothing to remove
    }

    bucket.erase(it);
  }
};

struct instruction {
  enum { insert, remove } action;
  std::string label;
  std::uint64_t value;
};

instruction parse_instruction(auto str) {
  instruction instr;
  auto it = std::ranges::find_first_of(str, "=-");
  instr.label = std::string(str.begin(), it);
  instr.action = (*it == '-') ? instruction::remove : instruction::insert;
  if (instr.action == instruction::insert) {
    instr.value = xmas::parse_int<std::uint64_t>({it + 1, str.end()});
  }
  return instr;
}

}  // namespace

std::uint64_t Day15::part1() {
  auto view = std::string_view(input.begin(), input.end() - 1) |
              std::ranges::views::split(',');

  return std::transform_reduce(
      std::execution::par_unseq, view.begin(), view.end(), std::uint64_t{0},
      std::plus<std::uint64_t>{}, [](auto substring) {
        return hash256(substring.begin(), substring.end());
      });
}

std::uint64_t Day15::part2() {
  auto view = std::string_view(input.begin(), input.end() - 1) |
              std::ranges::views::split(',');

  std::vector<instruction> s;
  std::transform(
      view.begin(), view.end(), std::back_insert_iterator(s),
      [](auto subs) -> instruction { return parse_instruction(subs); });

  hash_map hm;

  for (auto &&inst : std::move(s)) {
    switch (inst.action) {
    case instruction::insert:
      hm.insert(std::forward<std::string>(inst.label),
                std::forward<std::uint64_t>(inst.value));
      continue;
    case instruction::remove:
      hm.remove(inst.label);
    }
  }

  xmas::views::iota<std::size_t> iota(256);

  return std::transform_reduce(std::execution::par_unseq, iota.begin(),
                               iota.end(), std::uint64_t{0},
                               std::plus<std::uint64_t>{},
                               [&buckets = hm.buckets](std::size_t bucket_id) {
                                 std::uint64_t lenses_score = 0;

                                 std::size_t pos = 1;
                                 for (auto &v : buckets[bucket_id]) {
                                   lenses_score += pos * v.second;
                                   ++pos;
                                 }

                                 return (bucket_id + 1) * lenses_score;
                               });
}