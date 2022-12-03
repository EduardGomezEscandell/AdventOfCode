package array

import (
	"math"
)

// FromChannel reads from a channel until it is closed, then returns a slice
// with all the values it read.
// Optional parameter Length(int) establishes a pre-allocated length.
// - Prealloc(int) preallocates a capacity for the array. Only relevant to performance.
// - MaxLen(int) sets how many values to read from the channel.
func FromChannel[T any](ch <-chan T, options ...optFunc) []T {
	// Default parameters
	op := opts{
		prealloc: 0,
		maxLen:   math.MaxInt,
	}
	// Applying optional parameters
	for _, setOption := range options {
		setOption(&op)
	}

	// Preallocating
	arr := make([]T, 0, op.prealloc)
	if op.maxLen == 0 {
		return arr
	}

	// Reading chanel
	var i int
	for a := range ch {
		arr = append(arr, a)
		i++
		if i >= op.maxLen {
			break
		}
	}

	return arr
}

type opts struct {
	prealloc int
	maxLen   int
}

type optFunc func(*opts)

// Prealloc is an optional parameter of FromChannel.
func Prealloc(ln int) optFunc { // nolint: revive
	return func(o *opts) {
		o.prealloc = ln
	}
}

// MaxLen is an optional parameter of FromChannel.
func MaxLen(ln int) optFunc { // nolint: revive
	return func(o *opts) {
		o.maxLen = ln
	}
}
