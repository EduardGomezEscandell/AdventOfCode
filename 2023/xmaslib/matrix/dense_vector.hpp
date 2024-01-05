#pragma once

#include "dense_algebra.hpp"

#include "../lazy_string/lazy_string.hpp"

#include <algorithm>
#include <cmath>
#include <initializer_list>
#include <type_traits>
#include <sstream>
#include <utility>
#include <vector>
#include <execution>

namespace xmas {

template <typename T>
class basic_vector {
public:
  basic_vector(basic_vector<T> const& other) : m_data(other.data()) {
  }

  basic_vector(std::initializer_list<T>&& data) :
      m_data(std::forward<std::initializer_list<T>>(data)) {
  }

  basic_vector(basic_vector&& other) {
    std::swap(m_data, other.m_data);
  }

  basic_vector& operator=(basic_vector<T> const& other) {
    std::ranges::copy(other.data(), data());
    return *this;
  }

  basic_vector& operator=(basic_vector&& other) {
    this->m_data = std::forward<std::vector<T>>(other.m_data);
    return *this;
  }

  explicit basic_vector(std::size_t size) : m_data(size) {
  }

  basic_vector(std::size_t size, T const& v) : m_data(size, v) {
  }

  template <typename Iterator>
  basic_vector(Iterator begin, Iterator end) : m_data(end - begin) {
    std::copy(std::execution::unseq, begin, end, m_data.begin());
  }

  std::size_t size() const noexcept {
    return m_data.size();
  }

  auto& data() noexcept {
    return m_data;
  }

  auto const& data() const noexcept {
    return m_data;
  }

  auto const& cdata() const noexcept {
    return m_data;
  }

  auto begin() {
    return data().begin();
  }
  auto begin() const {
    return cbegin();
  }
  auto cbegin() const {
    return data().cbegin();
  }
  auto rbegin() {
    return data().rbegin();
  }
  auto rbegin() const {
    return rcbegin();
  }
  auto rcbegin() const {
    return data().rcbegin();
  }

  auto end() {
    return data().end();
  }
  auto end() const {
    return cend();
  }
  auto cend() const {
    return data().cend();
  }
  auto rend() {
    return data().rend();
  }
  auto rend() const {
    return rcend();
  }
  auto rcend() const {
    return data().rcend();
  }

  basic_vector& operator+=(basic_vector const& other) {
    elementwise(m_data, other.m_data, m_data, [](T t, T o) { return t + o; });
    return *this;
  }

  basic_vector& operator-=(basic_vector const& other) {
    elementwise(m_data, other.m_data, m_data, [](T t, T o) { return t - o; });
    return *this;
  }

  basic_vector operator+(basic_vector const& other) const {
    basic_vector<T> out(size());
    elementwise(m_data, other.m_data, out, [](T t, T o) { return t + o; });
    return out;
  }

  basic_vector operator-(basic_vector const& other) const {
    basic_vector<T> out(size());
    elementwise(m_data, other.m_data, out, [](T t, T o) { return t - o; });
    return out;
  }

  basic_vector& operator*=(T a) {
    elementwise(data(), data(), [a](T t) { return a * t; });
    return *this;
  }

  basic_vector<T> operator*(T a) const {
    auto out = basic_vector<T>(size());
    elementwise(cdata(), out.data(), [a](T t) { return a * t; });
    return out;
  }

  basic_vector& operator/=(T a) {
    elementwise(data(), data(), [a](T t) { return t / a; });
    return *this;
  }

  basic_vector<T> operator/(T a) const {
    auto out = basic_vector<T>(size());
    elementwise(cdata(), out.data(), [a](T t) { return t / a; });
    return out;
  }

  T& operator[](std::size_t idx) {
    return data()[idx];
  }

  T operator[](std::size_t idx) const {
    return data()[idx];
  }

  auto norm() const {
    return std::sqrt(norm2());
  }

  auto norm2() const {
    return algebra::inner<T>(*this, *this);
  }

  void normalize() {
    *this /= norm();
  }

  basic_vector normalized() const {
    return *this / norm();
  }

  auto format(std::string_view fmt = "{:>4}") const {
    return lazy_string([this, fmt] {
      std::stringstream ss;
      ss << '[';
      for (T t : cdata()) {
        ss << ' ' << std::vformat(fmt, std::make_format_args(t));
      }
      ss << " ]";
      return std::move(ss).str();
    });
  }

private:
  std::vector<T> m_data;

  static void elementwise(auto const& in_left, auto const& in_right, auto& out, auto&& op) {
    assert(in_left.size() <= in_right.size());
    assert(in_left.size() <= out.size());
    std::transform(
      std::execution::unseq, in_left.begin(), in_left.end(), in_right.begin(), out.begin(), op);
  }

  static void elementwise(auto const& in, auto& out, auto&& op) {
    assert(in.size() <= out.size());
    std::transform(std::execution::unseq, in.begin(), in.end(), out.begin(), op);
  }
};

using vector = basic_vector<float>;

template <typename T>
basic_vector<T> operator*(T a, basic_vector<T> const& v) {
  return v * a;
}

}