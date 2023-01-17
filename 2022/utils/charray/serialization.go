package charray

import (
	"context"
	"math"
)

// Serialize returns a channel that reads from array arr.
func Serialize[T any](ctx context.Context, arr []T, channelCapacity int) <-chan T {
	if ctx == nil {
		panic("nil context")
	}

	ch := make(chan T, channelCapacity)
	go func() {
		defer close(ch)
		for _, a := range arr {
			// This first select ensures that in case of a tie,
			// the cancel takes priority
			select {
			case <-ctx.Done():
				return
			default:
			}

			select {
			case <-ctx.Done():
				return
			case ch <- a:
			}
		}
	}()

	return ch
}

// Deserialize reads from a channel until it is closed, then returns a slice
// with all the values it read.
// Optional parameter Length(int) establishes a pre-allocated length.
// - Prealloc(int) preallocates a capacity for the array. Only relevant to performance.
// - MaxLen(int) sets how many values to read from the channel.
func Deserialize[T any](ch <-chan T, options ...optFunc) []T {
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

// Prealloc is an optional parameter of ToArray.
func Prealloc(ln int) optFunc { // nolint: revive
	return func(o *opts) {
		o.prealloc = ln
	}
}

// MaxLen is an optional parameter of ToArray.
func MaxLen(ln int) optFunc { // nolint: revive
	return func(o *opts) {
		o.maxLen = ln
	}
}
