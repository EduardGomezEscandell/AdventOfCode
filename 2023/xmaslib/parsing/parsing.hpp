#include <string_view>
#include <type_traits>
#include <vector>

namespace xmas {

template <std::integral T>
[[nodiscard]] T parse_int(std::string_view str) {
  T x{};
  for (auto ch : str) {
    x *= 10;
    x += ch - '0';
  }
  return x;
}

template <std::integral T>
[[nodiscard]] T parse_hex(std::string_view str) {
  T x{};
  for (auto ch : str) {
    x *= 0x10;
    if (ch >= '0' && ch <= '9') {
      x += ch - '0';
    }
    if (ch >= 'a' && ch <= 'f') {
      x += (ch - 'a') + 0xa;
    }
  }
  return x;
}

template <std::integral T, typename Iterator>
[[nodiscard]] std::pair<Iterator, T> parse_int_until_break(Iterator begin, Iterator end) {
  T x{};
  for (; begin != end; ++begin) {
    if (*begin < '0' || *begin > '9') {
      return {begin, x};
    }

    x *= 10;
    x += *begin - '0';
  }
  return {end, x};
}

// parse_ints parses a list of integers inbetween other characters
// "534<dv13213zv  5643" -> [534, 13213, 5343]
template <std::integral T>
[[nodiscard]] std::vector<T> parse_ints(std::string_view str) {
  std::vector<T> out;
  out.reserve(str.size());

  T n = 0;               // Current value of number
  bool parsing = false;  // Are we parsing a number?
  bool negative = false; // Was there a negative sign?

  for (auto ch : str) {
    if constexpr (std::is_signed_v<T>) {
      if (!parsing && ch == '-') {
        negative = true;
        parsing = true;
        continue;
      }
    }

    if (ch >= '0' && ch <= '9') {
      parsing = true;
      n = 10 * n + T(ch - '0');
      continue;
    }

    if (parsing) {
      if constexpr (std::is_signed_v<T>) {
        if (negative) {
          n *= -1;
        }
      }
      out.push_back(n);
    }

    n = 0;
    parsing = false;
    negative = false;
  }

  if (parsing) {
    if constexpr (std::is_signed_v<T>) {
      if (negative) {
        n *= -1;
      }
    }
    out.push_back(n);
  }

  out.shrink_to_fit();
  return out;
}

} // namespace xmas