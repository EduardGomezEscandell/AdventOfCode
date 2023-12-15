#include "day14.hpp"

#include "xmaslib/iota/iota.hpp"
#include "xmaslib/log/log.hpp"
#include "xmaslib/matrix/text_matrix.hpp"

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

std::uint64_t Day14::part2() { throw std::runtime_error("Not implemented"); }