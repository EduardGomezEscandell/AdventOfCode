#include "day14.hpp"

#include "xmaslib/iota/iota.hpp"
#include "xmaslib/log/log.hpp"
#include "xmaslib/lru/lru.hpp"
#include "xmaslib/matrix/text_matrix.hpp"

#include <algorithm>
#include <cstddef>
#include <execution>
#include <functional>
#include <numeric>
#include <stdexcept>

void Day14::load() {
  xmas::solution::load();

  // Trailing newline is necessary
  if (this->input.back() != '\n') {
    this->input.push_back('\n');
  }
}

std::uint64_t Day14::part1() {
  xmas::views::text_matrix matrix(input);

  xmas::views::iota<std::size_t> iota(matrix.ncols());
  return std::transform_reduce(
      std::execution::seq, iota.begin(), iota.end(), std::uint64_t{0},
      std::plus<std::uint64_t>{}, [&matrix](std::size_t j) -> std::uint64_t {
        auto col = matrix.col(j);

        std::size_t score = 0;
        std::size_t next_score = matrix.nrows();
        for (std::size_t i = 0; i != matrix.nrows(); ++i) {
          switch (col[i]) {
          case '.':
            continue;
          case '#':
            next_score = matrix.nrows() - i - 1;
            continue;
          case 'O':
            score += next_score;
            --next_score;
          }
        }

        return score;
      });
}

namespace {

#define def_flip_func(name, axis, reverse)                                     \
  void name(xmas::views::text_matrix &matrix) {                                \
    xmas::views::iota<std::size_t> iota(matrix.n##axis##s());                  \
    std::for_each(std::execution::seq, iota.begin(), iota.end(),               \
                  [&matrix](std::size_t j) {                                   \
                    auto axis = matrix.axis(j);                                \
                    auto fall_to = axis.reverse##begin();                      \
                    for (auto it = fall_to; it != axis.reverse##end(); ++it) { \
                      switch (*it) {                                           \
                      case '.':                                                \
                        break;                                                 \
                      case '#':                                                \
                        fall_to = it + 1;                                      \
                        break;                                                 \
                      case 'O':                                                \
                        *it = '.';                                             \
                        *fall_to = 'O';                                        \
                        ++fall_to;                                             \
                      }                                                        \
                    }                                                          \
                  });                                                          \
  }

def_flip_func(flip_north, col, );
def_flip_func(flip_south, col, r);
def_flip_func(flip_west, row, );
def_flip_func(flip_east, row, r);

void loop_the_loop(xmas::views::text_matrix &matrix) {
  flip_north(matrix);
  flip_west(matrix);
  flip_south(matrix);
  flip_east(matrix);
}

std::uint64_t compute_load(xmas::views::text_matrix &matrix) {
  xmas::views::iota<std::size_t> iota(matrix.nrows());
  return std::transform_reduce(
      std::execution::seq, iota.begin(), iota.end(), std::uint64_t{0},
      std::plus<std::uint64_t>{}, [&matrix](std::size_t i) {
        auto row = matrix.row(i);
        const std::size_t weight = matrix.nrows() - i;
        return std::transform_reduce(row.begin(), row.end(), std::uint64_t{0},
                                     std::plus<std::uint64_t>{},
                                     [weight](char c) -> std::uint64_t {
                                       return c == 'O' ? weight : 0u;
                                     });
      });
}

} // namespace

std::uint64_t Day14::part2() {
  auto raw{input};
  constexpr std::size_t N = 1000000000;

  xmas::views::text_matrix matrix(raw);
  xmas::lru_cache<std::uint64_t, std::size_t> history(128);
  //                                                  ^^^
  // This number needs to be larger that the cycle we're trying to capture.

  std::size_t cycle_size = 0;

  std::size_t i = 1;
  // Doing at most 1e6 iterations before giving up
  for (; i < 1'000'000; ++i) {
    loop_the_loop(matrix);

    auto hash = std::hash<std::string>{}(raw);
    xlog::debug("Iteration {} -> {}", i, hash);

    auto opt = history.get(hash);
    if (!opt.has_value()) {
      history.set(hash, i);
      continue;
    }

    cycle_size = i - *opt;
    break;
  }

  if (cycle_size == 0) {
    throw std::runtime_error("No cycle detected");
  }
  xlog::debug("Detected a cycle after {}  iterations", i);
  xlog::debug("The cycle is {} iterations long", cycle_size);

  // Fast-forward cycle
  std::size_t remaining_cycles = (N - i) / cycle_size;
  i += remaining_cycles * cycle_size;

  xlog::debug("Fast-forwarding to iteration {}", i);

  for (; i < N; ++i) {
    loop_the_loop(matrix);
  }

  return compute_load(matrix);
}