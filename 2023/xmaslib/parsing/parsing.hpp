#include <string_view>
#include <vector>

namespace xmas {

template <std::integral T> [[nodiscard]] T parse_int(std::string_view str) {
  T x{};
  for (auto ch : str) {
    x *= 10;
    x += ch - '0';
  }
  return x;
}

// parse_ints parses a list of integers inbetween other characters
// "534<dv13213zv  5643" -> [534, 13213, 5343]
template <std::integral T>
[[nodiscard]] std::vector<T> parse_ints(std::string_view str) {
  std::vector<T> out;
  out.reserve(str.size());

  T n = 0;              // Current value of number
  bool parsing = false; // Are we parsing a number?

  for (auto ch : str) {
    if (ch >= '0' && ch <= '9') {
      parsing = true;
      n = 10 * n + T(ch - '0');
      continue;
    }

    if (parsing) {
      out.push_back(n);
    }

    n = 0;
    parsing = false;
  }

  if (parsing) {
    out.push_back(n);
  }

  out.shrink_to_fit();
  return out;
}

} // namespace xmas