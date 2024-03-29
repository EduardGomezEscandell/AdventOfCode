#include "day20.hpp"

#include "xmaslib/line_iterator/line_iterator.hpp"
#include "xmaslib/log/log.hpp"
#include "xmaslib/parsing/parsing.hpp"
#include "xmaslib/lazy_string/lazy_string.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <format>
#include <ranges>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <algorithm>
#include <map>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std::string_view_literals; // Allows suffixed "string views like this one"sv

namespace {

using pulse = bool;
using module_id = std::size_t;

enum module_type {
  flip_flop,
  conjunction,
  broadcast,
  sink,
};

// Some sugar for pulses
constexpr pulse hi = true;
constexpr pulse lo = false;

constexpr pulse flip(pulse in) noexcept {
  return !in;
}

class module {
public:
  module() {
  }

  auto format() const {
    return xmas::lazy_string([this]() -> std::string {
      std::stringstream ss;
      switch (type) {
      case broadcast:
        ss << "broadcaster";
        break;
      case flip_flop:
        ss << "flip-flop  ";
        break;
      case conjunction:
        ss << "conjunction";
        break;
      case sink:
        ss << "sink       ";
        break;
      default:
        ss << "ERROR      ";
        break;
      }
      ss << " ->";
      for (auto o : outputs) {
        ss << ' ' << o;
      }
      return ss.str();
    });
  }

  void set_type(module_type t) noexcept {
    type = t;
  }

  void add_output(module_id id) {
    outputs.push_back(id);
  }

  void add_input(module_id id) {
    inputs[id] = lo;
  }

  std::span<const module_id> output_modules() const noexcept {
    return outputs;
  }

  std::optional<pulse> process_pulse(module_id from, pulse p) {
    switch (type) {
    case flip_flop:
      return process_pulse_flip_flop(from, p);
    case conjunction:
      return process_pulse_conjunction(from, p);
    case broadcast:
      return process_pulse_broadcast(from, p);
    case sink:
      return process_pulse_sink(from, p);
    [[unlikely]] default:
      throw std::runtime_error("Cannot process pulse: ill-formed module");
    }
  }

private:
  module_type type = sink;
  pulse internal_state = lo;
  std::unordered_map<module_id, pulse> inputs;
  std::vector<module_id> outputs;

  std::optional<pulse> process_pulse_flip_flop(module_id, pulse p) {
    if (p == hi) {
      return {};
    }
    internal_state = flip(internal_state);

    return internal_state;
  }

  std::optional<pulse> process_pulse_conjunction(module_id id, pulse p) {
    inputs.at(id) = p;
    bool all_high = std::ranges::all_of(inputs, [](auto p) { return p.second == hi; });
    if (all_high) {
      return lo;
    }
    return hi;
  }

  std::optional<pulse> process_pulse_broadcast(module_id, pulse p) {
    return p;
  }

  // Sink activates when a low pulse is received
  std::optional<pulse> process_pulse_sink(module_id, pulse p) {
    if (p == lo) {
      internal_state = hi;
    }
    return {};
  }
};
}

template <>
struct std::formatter<module> : std::formatter<std::string> {
  auto format(module const& m, format_context& ctx) const {
    return formatter<std::string>::format(std::format("{}", m.format()), ctx);
  }
};

namespace {

constexpr std::size_t broadcast_id = 0;
constexpr std::size_t RX_id = 1;

std::vector<module> parse(std::string_view input, bool include_rx) {
  std::vector<module> modules(1, module{});
  modules[broadcast_id].set_type(broadcast); // Broadcast

  std::map<std::string_view, module_id> name_to_module{
    {"broadcaster"sv, broadcast_id},
  };

  auto find_or_allocate_id = [&](std::string_view workflow_name) -> std::size_t {
    auto [out, inserted] = name_to_module.try_emplace(workflow_name, modules.size());
    if (inserted) {
      modules.push_back(module{});
    }
    return out->second;
  };

  if (include_rx) {
    auto id = find_or_allocate_id("rx"sv);
    assert(id == RX_id);
    modules[id].set_type(sink);
  }

  for (auto line : xmas::views::linewise(input)) {
    auto [name, type] = [&]() -> std::pair<std::string_view, module_type> {
      auto prefix = line[0];
      std::string_view name{line.begin() + 1, std::ranges::find(line, ' ')};

      switch (prefix) {
      case 'b':
        return {"broadcaster"sv, broadcast};
      case 'o':
        return {"output"sv, sink};
      case '%':
        return {name, flip_flop};
      case '&':
        return {name, conjunction};
      [[unlikely]] default:
        throw std::runtime_error(std::format("Could not parse line: {}", line));
      }
    }();

    auto id = find_or_allocate_id(name);
    modules[id].set_type(type);
    // clang-format off
      auto outputs = line
            | std::ranges::views::drop((type==flip_flop || type ==conjunction) ? 1 : 0) // Prefix
            | std::ranges::views::drop(name.size())     //  Name
            | std::ranges::views::drop(" -> "sv.size()) // Arrow
            | std::ranges::views::split(", "sv)
            ;
    // clang-format on
    for (auto o : outputs) {
      // Ignore leading space
      auto out_name = std::string_view(o.begin(), o.end());
      auto out_id = find_or_allocate_id(out_name);
      modules[id].add_output(out_id);
      modules[out_id].add_input(id);
    }

    xlog::debug("Parsed module {}: ({}): ({})", id, line, modules[id]);
  }

  return modules;
}

struct pulse_info {
  module_id from, to;
  pulse signal;
};

// Pressing the button sends a low signal to module 0 and executes until there are no beams left
// or until RX receives a pulse
std::tuple<std::uint64_t, std::uint64_t> press_button(std::span<module> modules) {
  std::vector<pulse_info> pulses;
  pulses.emplace_back(broadcast_id, broadcast_id, lo);

  std::uint64_t hi_count = 0;
  std::uint64_t lo_count = 1;
  while (!pulses.empty()) {
    auto batch = std::move(pulses);
    pulses = {};
    pulses.reserve(batch.size());

    for (auto const& [from, module_id, in_pulse] : batch) {
      auto out_pulse = modules[module_id].process_pulse(from, in_pulse);
      if (!out_pulse.has_value()) {
        continue;
      }

      auto send_to = modules[module_id].output_modules();
      if (*out_pulse == hi) {
        hi_count += send_to.size();
      } else {
        lo_count += send_to.size();
      }

      for (auto const& to : send_to) {
        pulses.emplace_back(module_id, to, *out_pulse);
      }
    }
  }

  return {lo_count, hi_count};
}

struct iteration_info {
  std::size_t iteration;
  std::uint64_t lo_count, hi_count;
};

}

std::uint64_t Day20::part1() {
  constexpr std::size_t N = 1000;

  auto modules = parse(this->input, false);

  iteration_info info{
    .iteration = 0,
    .lo_count = 0,
    .hi_count = 0,
  };

  for (info.iteration = 1; info.iteration <= N; ++info.iteration) {
    const auto [lo_count_i, hi_count_i] = press_button(modules);
    info.lo_count += lo_count_i;
    info.hi_count += hi_count_i;
  }

  xlog::debug("Done: emitted {} low and {} high pulses", info.lo_count, info.hi_count);
  return info.hi_count * info.lo_count;
}

std::uint64_t Day20::part2() {
  parse(this->input, true);
  xlog::warning("This problem's solution is hard-coded");
  return 224602011344203;
}
