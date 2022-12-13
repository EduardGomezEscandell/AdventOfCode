// Package charray is used to implement operations usually applied to
// arrays, but applied to channels instead. It's an async approach to
// lazy evaluation.
package charray

import (
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/channel"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
)

// Map applies function f:T->O element-wise to generate another
// channeled array <-chan O of the same length.
func Map[T, O any](in <-chan T, f func(T) O) <-chan O {
	out := make(chan O, cap(in))
	go func() {
		defer close(out)
		defer channel.Exhaust(in)
		for v := range in {
			out <- f(v)
		}
	}()
	return out
}

// Generate generates a channel array of length len,
// where arr[i] = f()
//
// The function will be called in sequential order.
// The channel is closed at the end.
//
// TODO: Cancelation.
func Generate[O any](len int, cap int, f func() O) <-chan O {
	out := make(chan O, cap)
	go func() {
		defer close(out)

		for i := 0; i < len; i++ {
			out <- f()
		}
	}()
	return out
}

// Reduce []T->O applies fold:MxT->M cummulatively, starting
// with the default value for M. The end result is
// equivalent to:
//
//	fold(fold(...fold(0, <-in), ..., <-in), <-in)
//
// where n is the length of arr
//
// Example use: Sum the values
//
//	Reduce(in, func(x,y int)int { return x+y }) # Option 1.
//	Reduce(in, fun.Add[int])                    # Option 2.
func Reduce[T, O any](in <-chan T, fold func(O, T) O, init O) O {
	o := init
	for v := range in {
		o = fold(o, v)
	}
	return o
}

// ZipWith takes two channel arrays of type <-chan L and <- chan R, and applies
// zip:LxR->O elementwise to produce a channel array of type <-chan O and length
// equal to the length of the shortest input. The output channel is closed at
// the end.
func ZipWith[F, S, O any](first <-chan F, second <-chan S, zip func(F, S) O) <-chan O {
	out := make(chan O, fun.Max(cap(first), cap(second)))
	go func() {
		defer close(out)
		defer channel.Exhaust(first)
		defer channel.Exhaust(second)

		for {
			f, okf := <-first
			s, oks := <-second
			if !okf || !oks {
				break
			}
			out <- zip(f, s)
		}
	}()
	return out
}

// AdjacentMap slides a window of size 2 across the channel array arr applying
// operator 'f' to produce a channel array of size |in| - 1.
func AdjacentMap[I, O any](in <-chan I, f func(I, I) O) <-chan O {
	out := make(chan O, cap(in))
	go func() {
		defer close(out)
		defer channel.Exhaust(in)

		prev := <-in
		for curr := range in {
			out <- f(prev, curr)
			prev = curr
		}
	}()
	return out
}

// Best traverses the channel array and tries to find the "best" entry.
// An entry x is the best if isBetter(x, y) is true for any value of y
// present in the array.
// Expected properties of isBetter:
//
//	Anticommutativity: isBetter(x,y) = !isBetter(y,x)
//	Total ordering:    isBetter(x,y), isBetter(y,z) <==> isBetter(x, z).
//
// Example use: return the maximum value in the list
//
//	Best(in, func(x, y int) bool { return x>y })
//	Best(in, fun.Gt)
//
// Complexity is O(|in|).
func Best[I any](in <-chan I, isBetter fun.Comparator[I]) (acc I) {
	for v := range in {
		if isBetter(v, acc) {
			acc = v
		}
	}
	return acc
}

// BestN returns the best N entries in the channel array in. An entry
// x is the best if isBetter(x, y) is true for any value of y present
// in the array.
// Expected properties of isBetter:
//
//	Anticommutativity: isBetter(x,y) = !isBetter(y,x)
//	Total ordering:    isBetter(x,y), isBetter(y,z) <=> isBetter(x, z).
//
// Example use: return the 3 largest values in the list
//
//	BestN(in, 3, func(x, y int) bool { return x>y })
//	BestN(in, 3, fun.Gt)
//
// Complexity is O(n·|in|).
func BestN[I any](in <-chan I, n uint, isBetter fun.Comparator[I]) []I {
	out := make([]I, 0, n)
	for i := uint(0); i < n; i++ {
		v, ok := <-in
		if !ok {
			break
		}
		out = append(out, v)
	}
	array.Sort(out, isBetter)

	for v := range in {
		array.UpdateBestN(n, out, v, isBetter)
	}

	return out
}

// Common finds all elements that two channel arrays have in common.
//
// The lists are expected to have been sorted with the comparator 'comp'.
// Two items a,b are considered equivalent if both comp(a,b) and comp(b,a)
// are false.
//
// It returns a slice with their common items. Items in the output list are
// repeated as many times as the smallest number of repetitions between the
// two lists.
//
// Complexity is O(|first| + |second|).
func Common[T any](first, second <-chan T, comp fun.Comparator[T]) <-chan T {
	out := make(chan T, fun.Max(cap(first), cap(second)))
	go func() {
		defer close(out)
		defer channel.Exhaust(first)
		defer channel.Exhaust(second)

		f, okf := <-first
		s, oks := <-second
		for okf && oks {
			// f preceedes s
			if comp(f, s) {
				f, okf = <-first
				continue
			}
			// f succeeds s
			if comp(s, f) {
				s, oks = <-second
				continue
			}
			// f == s
			out <- f
			f, okf = <-first
			s, oks = <-second
		}
	}()
	return out
}

// Unique filters in so that all repeated items are dropped.
// Returns a channel with the unique values.
//
// Assumes the input is sorted in such a way that equal(x,y)
// implies all items between x, and y will also compare equal.
//
// Sorting with < and applying == will work.
//
// Complexity is O(|in|).
func Unique[T any](in <-chan T, equal fun.Comparator[T]) <-chan T {
	out := make(chan T, cap(in))
	go func() {
		defer close(out)
		defer channel.Exhaust(in)

		prev, ok := <-in
		if !ok {
			return
		}
		out <- prev

		for v := range in {
			if equal(v, prev) {
				continue
			}
			prev = v
			out <- v
		}
	}()
	return out
}

// Multiplex takes a channel array and produces n channels. Each value read
// from the input will be sent to the output returned from selection(value).
func Multiplex[T any](in <-chan T, n int, selection func(T) int) []<-chan T {
	outs := array.Generate(n, func() chan T { return make(chan T, cap(in)) })

	go func() {
		defer channel.Exhaust(in)
		defer array.Foreach(outs, func(ch *chan T) { close(*ch) })

		for v := range in {
			idx := selection(v)
			outs[idx] <- v
		}
	}()
	return array.Map(outs, channel.ConvertToRecieveOnly[T])
}

// Concat concatenates two channeled arrays.
func Concat[T any](a <-chan T, b <-chan T) <-chan T {
	out := make(chan T, fun.Max(cap(a), cap(b)))
	go func() {
		defer close(out)

		go func() {
			defer channel.Exhaust(a)
			defer channel.Exhaust(b)
			defer close(out)
			for v := range a {
				out <- v
			}
			for v := range b {
				out <- v
			}
		}()
	}()
	return out
}
