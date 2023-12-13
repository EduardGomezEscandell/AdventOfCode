#include <algorithm>
#include <execution>
#include <iterator>
#include <string_view>

namespace xmas {
namespace views {

struct linewise {
  class iterator;

  linewise(std::string_view ss) : view{ss} {}

  iterator cbegin() const { return {view.begin(), view.end()}; }
  iterator cend() const { return {view.end(), view.end()}; }

  auto begin() const { return cbegin(); }
  auto end() const { return cend(); }

private:
  std::string_view view;

public:
  class iterator {
    friend linewise;

  public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = std::string_view;
    using pointer = value_type *;
    using reference = value_type &;

    bool operator==(iterator const &other) {
      return this->start == other.start;
    }

    bool operator!=(iterator const &other) { return !(*this == other); }

    iterator operator++() {
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

    iterator(base start, base end) : start(start), endl(start), endtext(end) {
      advance_endl();
    }

    base start;
    base endl;
    base endtext;
  };
};

} // namespace views
} // namespace xmas