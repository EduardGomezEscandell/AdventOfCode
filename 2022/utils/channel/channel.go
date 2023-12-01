// Package channel implements utilities relating to channels.
package channel

import (
	"context"

	"github.com/EduardGomezEscandell/algo/algo"
	"github.com/EduardGomezEscandell/algo/utils"
)

// Split takes an input channel and creates n channels such that every value
// that is sent to the input can be read from all the output channels. The
// output channels will have the same capacity as the input one.
//
// The order in which the output channels are read from is not important, but
// no new value will be read from input until the previous one has been written
// to all outputs.
//
// This means that one output can only read cap(input) values ahead of the most
// delayed output.
func Split[T any](ctx context.Context, input <-chan T, n int) []<-chan T {
	if ctx == nil {
		panic("nil context")
	}

	// Create slice of output channels
	capacity := cap(input)
	outputs := algo.Generate(n, func() chan T { return make(chan T, capacity) })

	// Splitting coroutine. Reads from input and sends it down all outputs.
	go func() {
		defer algo.Foreach(outputs, func(o *chan T) { close(*o) })

		for {
			// Reading from the channel
			var value T
			var ok bool
			select {
			case <-ctx.Done():
				return
			case value, ok = <-input:
			}

			if !ok {
				return
			}

			// Asyncronously sending value down all channels
			failures := algo.Map(outputs, func(out chan T) <-chan bool {
				failed := make(chan bool)
				go func() {
					select {
					case out <- value:
						failed <- false
					case <-ctx.Done():
						failed <- true
					}
					close(failed)
				}()
				return failed
			})
			// Barrier: collecting failures (and ending if one failed)
			anyFailed := algo.MapReduce(failures, Recv[bool], utils.Or, false)
			if anyFailed {
				return
			}
		}
	}()

	return algo.Map(outputs, func(ch chan T) <-chan T { return ch })
}

// Recv is a wrapper around <-ch.
func Recv[T any](ch <-chan T) T {
	return <-ch
}

// RecvOk is a rapper around <-ch, returning a struct
// with the value and the status of the channel.
func RecvOk[T any](ch <-chan T) Recvd[T] {
	v, ok := <-ch
	return Recvd[T]{V: v, Ok: ok}
}

// Recvd is used to pack the value read and
// and the status of a channel.
type Recvd[T any] struct {
	V  T
	Ok bool
}

// Send is a wrapper around ch <- val.
func Send[T any](ch chan<- T, val T) {
	ch <- val
}

// Close is a wrapper around close(ch).
func Close[T any](ch chan T) {
	close(ch)
}

// ConvertToRecieveOnly converts chan into <-chan.
func ConvertToRecieveOnly[T any](ch chan T) <-chan T {
	return ch
}

// Exhaust pulls and ignores all the values in a channel.
func Exhaust[T any](ch <-chan T) {
	//nolint:revive // Yes, I want it empty
	for range ch {
	}
}
