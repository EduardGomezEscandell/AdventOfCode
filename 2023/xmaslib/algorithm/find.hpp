#pragma once

#include <algorithm>
#include <cstddef>
#include <execution>
#include <iterator>
#include <utility>
#include <vector>

namespace internal {

auto ceil_divide(auto p, auto q) {
  return p / q + (p % q != 0);
}

template <typename Iterator, typename BinaryPredicate>
Iterator find_sorted_single_threaded(Iterator begin, Iterator end, BinaryPredicate&& pred) {
  if (end - begin <= 1) {
    return begin;
  }

  auto mid = begin + (end - begin) / 2;
  if (pred(mid)) {
    return find_sorted_single_threaded(mid, end, pred);
  }
  return find_sorted_single_threaded(begin, mid + 1, pred);
}

inline std::vector<std::pair<std::size_t, std::size_t>> workload_distribution(
  std::size_t nelems, std::size_t ncores) {
  const auto elems_per_core = nelems / ncores;
  const auto left_over = nelems % ncores;

  std::vector<std::size_t> sizes(ncores, elems_per_core);
  std::transform(std::execution::unseq, sizes.begin(),
    sizes.begin() + static_cast<std::ptrdiff_t>(left_over), sizes.begin(),
    [](auto s) { return s + 1; });

  std::vector<std::pair<std::size_t, std::size_t>> boundaries(ncores);
  std::size_t acc = 0;
  for (std::size_t i = 0; i < ncores; ++i) {
    boundaries[i] = std::make_pair(acc, acc += sizes[i]);
  }

  return boundaries;
}

template <typename Iterator, typename BinaryPredicate>
Iterator find_sorted_parallel(Iterator begin, Iterator end, BinaryPredicate&& pred) {
  const auto ncores = std::thread::hardware_concurrency();
  const auto nelems = static_cast<std::size_t>(end - begin);

  if (nelems < ncores) {
    return find_sorted_single_threaded(begin, end, std::forward<BinaryPredicate>(pred));
  }

  auto segments = workload_distribution(nelems, ncores);
  std::vector<Iterator> results(ncores);

  std::transform(std::execution::seq, segments.begin(), segments.end(), results.begin(),
    [=](auto s) { return find_sorted_parallel(begin + s.first, end + s.second, pred); });

  auto jt = find_sorted_single_threaded(
    results.begin(), results.end(), [=](Iterator it) { return pred(*it); });
  if (jt == results.end()) {
    return end;
  }

  return *jt;
}

}

// Given a range partitioned such that its prefix does not match the predicate, and its suffix which
// does, find_sorted returns the first entry to fail the predicate.
template <typename ExecutionPolicy, typename Iterator, typename BinaryPredicate>
Iterator find_sorted(ExecutionPolicy&&, Iterator begin, Iterator end, BinaryPredicate&& pred) {
  if constexpr (!std::is_same<typename std::iterator_traits<Iterator>::iterator_category,
                  std::random_access_iterator_tag>::value) {
    return std::find_if(begin, end, std::forward(pred));
  }

  if constexpr (std::is_same<ExecutionPolicy, decltype(std::execution::seq)>::value) {
    return internal::find_sorted_single_threaded(begin, end, std::forward<BinaryPredicate>(pred));
  }

  if constexpr (std::is_same<ExecutionPolicy, decltype(std::execution::unseq)>::value) {
    return internal::find_sorted_single_threaded(begin, end, std::forward<BinaryPredicate>(pred));
  }

  if constexpr (std::is_same<ExecutionPolicy, decltype(std::execution::par)>::value) {
    return internal::find_sorted_parallel(begin, end, std::forward<BinaryPredicate>(pred));
  }

  if constexpr (std::is_same<ExecutionPolicy, decltype(std::execution::par_unseq)>::value) {
    return internal::find_sorted_parallel(begin, end, std::forward<BinaryPredicate>(pred));
  }

  static_assert(false, "Invalid execution policy");
}
