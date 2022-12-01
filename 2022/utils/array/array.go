// Package array provides generic array operations to facilitate
// array-based operations.
package array

import "github.com/EduardGomezEscandell/AdventOfCode/2022/utils/generics"

type number generics.Number

// Reduce appies the fold operation from the left.
func Reduce[T, M number](vector []T, fold func(M, T) M) M {
	var acc M
	for _, v := range vector {
		acc = fold(acc, v)
	}
	return acc
}

// ScanReduce scans and the reduces an array.
//
//	Reduce(Scan(arr, unary), fold)
//
// Note: the intermediate array is not stored into memory, making this operation faster than
// the proposed equivalent one.
func ScanReduce[T, M, I any](arr []T, unary func(T) I, fold func(M, I) M) M {
	var acc M
	for _, a := range arr {
		acc = fold(acc, unary(a))
	}
	return acc
}

// AdjacentReduce slides a window of size 2 across the array arr applying operator 'merge',
// producing an array of size len(arr)-1. The latter array is then reduced with the
// fold operator.
//
// Equivalent to:
//
//	Reduce(AdjacentScan(arr, merge), fold)
//
// Note: the intermediate array is not stored into memory, making this operation faster than
// the proposed equivalent one.
func AdjacentReduce[T, A, I number](arr []T, merge func(T, T) I, fold func(A, I) A) A {
	var acc A
	if len(arr) < 2 {
		return acc
	}

	for i := 1; i < len(arr); i++ {
		acc = fold(acc, merge(arr[i-1], arr[i]))
	}
	return acc
}

// Scan applies function f element-wise to generate another array of the same size.
func Scan[T, M any](arr []T, f func(T) M) []M {
	if len(arr) < 1 {
		return []M{}
	}

	out := make([]M, len(arr))
	for i, a := range arr {
		out[i] = f(a)
	}
	return out
}

// AdjacentScan slides a window of size 2 across the array arr applying operator 'f'
// to produce an array of size len(arr)-1.
func AdjacentScan[T, M number](arr []T, f func(T, T) M) []M {
	if len(arr) < 2 {
		return []M{}
	}

	out := make([]M, len(arr)-1)
	for i := 1; i < len(arr); i++ {
		out[i-1] = f(arr[i-1], arr[i])
	}
	return out
}
