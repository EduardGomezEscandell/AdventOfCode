#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <ratio>
#include <stdexcept>

namespace xmas {

class solution {
public:
  virtual int day() = 0;

  virtual void set_input(std::string_view path);
  virtual void load();
  virtual bool run(bool verbose) noexcept;

  // Same as standard library
  using duration = std::chrono::duration<long, std::ratio<1, 1000000000>>;
  virtual duration time() const;

protected:
  virtual std::int64_t part1() { throw std::runtime_error("not implemented"); }
  virtual std::int64_t part2() { throw std::runtime_error("not implemented"); }

  std::string input;

private:
  std::string data_path;

  std::optional<std::int64_t> p1;
  std::optional<std::int64_t> p2;

  duration time_p1;
  duration time_p2;
};

} // namespace xmas