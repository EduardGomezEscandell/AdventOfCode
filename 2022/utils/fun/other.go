package fun

// Append is a wrapper around the append built-in
func Append[T any](acc []T, t T) []T {
	return append(acc, t)
}
