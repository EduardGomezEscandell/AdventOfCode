package charray

import (
	"context"
)

// FromArray returns a channel that reads from array arr.
func FromArray[T any](ctx context.Context, arr []T, channelCapacity int) <-chan T {
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
