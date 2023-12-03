#pragma once

#include <cstdint>
#include <optional>
#include <stdexcept>

namespace xmas {

class solution {
public:
  virtual int day() = 0;

  virtual void set_input(std::string_view path);
  virtual void load();
  virtual bool run() noexcept;

protected:
  virtual std::int64_t part1() { throw std::runtime_error("not implemented"); }
  virtual std::int64_t part2() { throw std::runtime_error("not implemented"); }

  std::string input;

private:
  std::string data_path;

  std::optional<std::int64_t> p1;
  std::optional<std::int64_t> p2;
};

} // namespace xmas