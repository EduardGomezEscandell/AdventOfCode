#include <algorithm>
#include <execution>
#include <iterator>
#include <string_view>

namespace xmas {

struct line_iterator {
  using iterator_category = std::forward_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = std::string_view;
  using pointer = value_type *;
  using reference = value_type &;

  static auto begin(std::string_view str) {
    auto it = line_iterator(str.begin(), str.end());
    it.advance_endl();
    return it;
  }

  static auto end(std::string_view str) {
    return line_iterator(str.end(), str.end());
  }

  bool operator==(line_iterator const &other) {
    return this->start == other.start;
  }

  bool operator!=(line_iterator const &other) { return !(*this == other); }

  line_iterator operator++() {
    start = endl;
    if (start == endtext) {
      return *this;
    }

    ++start;
    advance_endl();

    return *this;
  }

  value_type operator*() const { return {start, endl}; }

private:
  using base = std::string_view::iterator;

  void advance_endl() {
    this->endl =
        std::find(std::execution::unseq, this->start, this->endtext, '\n');
  }

  line_iterator(base start, base end)
      : start(start), endl(start), endtext(end) {
    advance_endl();
  }

  base start;
  base endl;
  base endtext;
};

struct line_range {
  line_range(std::string_view ss) : view{ss} {}

  auto begin() const { return line_iterator::begin(view); }
  auto end() const { return line_iterator::end(view); }

private:
  std::string_view view;
};

} // namespace xmas