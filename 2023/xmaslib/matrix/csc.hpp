#pragma once

#include "../functional/functional.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <execution>
#include <iterator>
#include <utility>
#include <vector>

namespace xmas {

// Compressed sparse column
template <typename T> struct csc_matrix {
private:
  std::size_t m_nrows, m_ncols;

  // Index in "cols" and "values" where that row begins
  std::vector<std::size_t> rows;

  // Columns where the value is non-zero
  std::vector<std::size_t> cols;

  // The data
  std::vector<T> data;

  T zero_value = T{};

public:
  class iterator;

  csc_matrix(std::size_t nrows, std::size_t ncols)
      : m_nrows(nrows), m_ncols(ncols), rows(nrows + 1, 0) {}

  csc_matrix(std::size_t nrows, std::size_t ncols, T &&zero_value)
      : m_nrows(nrows), m_ncols(ncols), rows(nrows + 1, 0),
        zero_value(std::forward<T &&>(zero_value)) {}

  std::size_t size() const { return data.size(); }
  std::size_t ncols() const noexcept { return m_ncols; }
  std::size_t nrows() const noexcept { return m_nrows; }

  // TODO: What if the value is 0?
  void set(std::size_t row, std::size_t col, T value) {
    assert(row < m_nrows);
    assert(col < m_ncols);
    auto begin = cols.begin() + static_cast<std::ptrdiff_t>(rows[row]);
    auto end = cols.begin() + static_cast<std::ptrdiff_t>(rows[row + 1]);

    auto it = std::find_if_not(begin, end, xmas::less_than(col));
    if (it == end || *it != col) {
      cols.insert(it, col);
      data.insert(it, value);
      ++rows[row + 1];
    } else {
      auto i = static_cast<std::size_t>(it - begin);
      data[i] = value;
    }
  }

  std::size_t get(std::size_t row, std::size_t col) const {
    assert(row < m_nrows);
    assert(col < m_ncols);
    auto begin = cols.begin() + static_cast<std::ptrdiff_t>(rows[row]);
    auto end = cols.begin() + static_cast<std::ptrdiff_t>(rows[row + 1]);

    auto it = std::find_if_not(begin, end, xmas::less_than(col));
    if (it == end || *it != col) {
      return zero_value;
    }
    return data[static_cast<std::size_t>(it - begin)];
  }

  // init_row fills a new row in bulk.
  void init_row(std::size_t row, std::vector<std::size_t> const &cols,
                std::vector<T> const &data) {
    const std::size_t data_size = data.size();
    assert(cols.size() == data_size);
    assert(row_size(row) == 0); // Row already initialized!

    std::for_each(
        std::execution::par_unseq,
        rows.begin() + static_cast<std::ptrdiff_t>(row + 1), rows.end(),
        [data_size](std::size_t &row_starts) { row_starts += data_size; });

    std::size_t idx = rows[row];
    insert_bulk(this->cols, idx, cols);
    insert_bulk(this->data, idx, data);
  }

  auto begin() { return iterator(*this, 0u, 0); }
  auto end() { return iterator(*this, m_nrows, 0); }

  std::size_t row_size(std::size_t row) const {
    assert(row < m_nrows);
    return rows[row + 1] - rows[row];
  }

  // Using uint8_t because vector<bool> is BAD! (Causes races in the transform)
  std::vector<std::uint8_t> row_density() const {
    std::vector<std::uint8_t> r(m_nrows);
    return std::transform(std::execution::par_unseq, rows.begin(),
                          rows.end() - 1, rows.begin() + 1, r.begin(),
                          [](std::size_t begin, std::size_t begin_next) {
                            return begin != begin_next ? 1 : 0;
                          });
    return r;
  }

  // Using uint8_t because vector<bool> is BAD! (Causes races in the transform)
  std::vector<std::uint8_t> col_density() const {
    std::vector<std::uint8_t> r(m_ncols, false);
    std::for_each(std::execution::par_unseq, cols.begin(), cols.end(),
                  [&r](std::size_t col) { r[col] = 1; });
    return r;
  }

  auto &raw_rows() { return rows; }
  auto &raw_columns() { return cols; }
  auto &raw_data() { return data; }

  // shrink removes trainling rows and columns from the matrix
  void shrink() {
    auto it = std::adjacent_find(rows.rbegin(), rows.rend(),
                                 [](std::size_t ibegin, std::size_t jbegin) {
                                   return ibegin != jbegin;
                                 });
    auto empty_trailing_rows = it - rows.rbegin();
    m_nrows = static_cast<std::size_t>(rows.size() - empty_trailing_rows);
    rows.resize(m_nrows);

    if (m_nrows == 0) {
      return;
    }

    m_ncols = 1 + *std::max_element(cols.begin(), cols.end());
  }

private:
  template <typename D>
  void insert_bulk(std::vector<D> &v, std::size_t pos,
                   std::vector<D> const &data) {
    assert(pos <= v.size());

    if (data.size() == 0) {
      return;
    }

    if (v.size() == 0) {
      v = data;
      return;
    }

    v.resize(v.size() + data.size());
    auto cp_begin = v.begin() + static_cast<std::ptrdiff_t>(pos);
    auto cp_end = cp_begin + static_cast<std::ptrdiff_t>(data.size());

    // Shift current data to the end
    std::copy_backward(cp_begin, cp_end, cp_end);
    // Copy new data
    std::copy(data.begin(), data.end(), cp_begin);
  }

public:
  class iterator {
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;

    struct value_type {
      std::size_t row;
      std::size_t col;
      T &value;
    };

    using pointer = void;
    using reference = value_type &;

    iterator(csc_matrix &src, std::size_t row, std::ptrdiff_t offset)
        : source{src}, row(row), offset(offset) {}

    iterator &operator++() noexcept {
      ++offset;
      if (row >= source.m_nrows) {
        return *this;
      }

      auto row_len = source.row_size(row);
      if (offset < static_cast<std::ptrdiff_t>(row_len)) {
        return *this;
      }

      // Advance until next non-empty row
      offset = 0;
      do {
        ++row;
        if (row >= source.m_nrows) {
          break;
        }
        row_len = source.row_size(row);
      } while (row_len == 0);

      return *this;
    }

    iterator operator++(int) noexcept {
      auto it = *this;
      ++*this;
      return it;
    }

    iterator &operator--() noexcept {
      --offset;
      if (offset >= 0) {
        return *this;
      }
      if (row == 0) {
        return *this;
      }

      std::size_t row_len = 0;
      while (row_len == 0) {
        --row;
        row_len = source.row_size(row);
        if (row == 0) {
          break;
        }
      }

      offset = row_len - 1;
      return *this;
    }

    iterator operator--(int) noexcept {
      auto it = *this;
      --*this;
      return it;
    }

    [[nodiscard]] bool operator==(iterator other) const noexcept {
      return row == other.row && offset == other.offset;
    }

    [[nodiscard]] bool operator!=(iterator other) const noexcept {
      return !(*this == other);
    }

    [[nodiscard]] std::strong_ordering
    operator<=>(iterator other) const noexcept {
      if (row != other.row) {
        return row <=> other.row;
      }
      return offset <=> other.offset;
    }

    [[nodiscard]] value_type operator*() const noexcept {
      std::size_t idx = source.rows[row] + offset;
      return value_type{
          .row = row,
          .col = source.cols[idx],
          .value = source.data[idx],
      };
    }

  private:
    csc_matrix &source;
    std::size_t row;
    std::ptrdiff_t offset;
  };
};

} // namespace xmas