namespace xmas {

template <typename T> class equals {
public:
  explicit constexpr equals(T value) : value(value) {}

  constexpr bool operator()(T const &other) const { return value == other; }

private:
  T value;
};

} // namespace xmas