#include <functional>
#include <string>
#include <format>

namespace xmas {
struct lazy_string {
  std::function<std::string()> generate;
};

} // namespace xmas

template <> struct std::formatter<xmas::lazy_string> : std::formatter<std::string> {
  auto format(xmas::lazy_string const &s, format_context &ctx) const {
    return formatter<std::string>::format(s.generate(), ctx);
  }
};