#pragma once

#include <algorithm>
#include <optional>
#include <unordered_map>
#include <vector>

namespace xmas {

/*
Least Recently Updated cache. A cache of the specified max_size. When a new
entry is added, the least recently updated entry is evicted.
*/
template <typename K, typename V> class lru_cache {
public:
  lru_cache(std::size_t max_size) : max_size(max_size), by_key(max_size) {}

  [[nodiscard]] std::optional<V> get(K const &k) const {
    if (auto it = by_key.find(k); it != by_key.end()) {
      return {data[it->second].value};
    }

    return {};
  }

  void set(K const &k, V const &v) {
    if (auto it = by_key.find(k); it != by_key.end()) {
      update(it->second, v);
      return;
    }

    add_new(k, v);
  }

private:
  auto heap_comp() {
    return [this](std::size_t i, std::size_t j) {
      return data[i].time_stamp > data[j].time_stamp;
    };
  }

  void add_new(K const &key, V const &value) {
    if (data.size() < max_size) {
      data.push_back(entry{.time_stamp = t++, .key = key, .value = value});
      by_age.push_back(data.size() - 1);
      by_key[key] = data.size() - 1;
      return;
    }

    std::pop_heap(by_age.begin(), by_age.end(), heap_comp());
    std::size_t reuse_index = by_age.back();
    by_key.erase(data[reuse_index].key);

    data[reuse_index] = entry{
        .time_stamp = t++,
        .key = key,
        .value = value,
    };

    by_key[key] = reuse_index;
    std::push_heap(by_age.begin(), by_age.end(), heap_comp());
  }

  void update(std::size_t idx, V const &value) {
    data[idx].time_stamp = t++;
    data[idx].value = value;

    std::make_heap(by_age.begin(), by_age.end(), heap_comp());
    return;
  }

  std::size_t t = 0;
  std::size_t max_size;

  using time_point = std::size_t;
  struct entry {
    time_point time_stamp;
    K key;
    V value;
  };

  std::unordered_map<K, std::size_t> by_key;
  std::vector<std::size_t> by_age;
  std::vector<entry> data;
};

} // namespace xmas