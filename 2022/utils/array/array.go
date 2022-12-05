// Package array provides generic array routines to facilitate
// array-based operations.
package array

import (
	"sort"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/generics"
)

type number generics.Number

// Map applies function f:T->O element-wise to generate another
// array []O of the same size.
func Map[T, O any](arr []T, f func(T) O) []O {
	if len(arr) < 1 {
		return []O{}
	}

	o := make([]O, len(arr))
	for i, a := range arr {
		o[i] = f(a)
	}
	return o
}

// Foreach applies non-pure function f:T element-wise t modify the array.
func Foreach[T any](arr []T, f func(*T)) {
	for i := 0; i < len(arr); i++ {
		f(&arr[i])
	}
}

// Generate generates an array of length len, where arr[i] = f()
// The function will be called in sequential order.
func Generate[T any](len int, f func() T) []T {
	arr := make([]T, 0, len)
	for i := 0; i < len; i++ {
		arr = append(arr, f())
	}
	return arr
}

// Reduce []T->O applies the function fold:MxT->M cummulatively,
// starting with the default value for M. The end result is
// equivalent to:
//
//	fold(fold(...fold(0, arr[0]), ..., arr[n-2]), arr[n-1])
//
// where n is the length of arr
//
// Example use: Sum the values
//
//	Reduce(arr, func(x,y int)int { return x+y }) # Option 1.
//	Reduce(arr, fun.Add[int])                    # Option 2.
func Reduce[T, O any](arr []T, fold func(O, T) O) O {
	var o O
	for _, a := range arr {
		o = fold(o, a)
	}
	return o
}

// MapReduce maps with the unary operator T->M, producing an
// intermediate array []M that is then reduced with fold:
// OxM->O.
//
// Equivalent to:
//
//	Reduce(Map(arr, unary), fold)
//
// Note: the intermediate array is not stored in memory.
func MapReduce[T, O, M any](arr []T, unary func(T) M, fold func(O, M) O) O {
	var o O
	for _, a := range arr {
		o = fold(o, unary(a))
	}
	return o
}

// ZipWith takes two arrays of type []L and []R, and applies zip:LxR->O
// elementwise to produce an array of type []O and length equal to the
// length of the shortest input.
func ZipWith[L, R, O any](first []L, second []R, f func(L, R) O) []O {
	ln := fun.Min(len(first), len(second))

	o := make([]O, ln)
	for i := 0; i < ln; i++ {
		o[i] = f(first[i], second[i])
	}
	return o
}

// ZipReduce takes two arrays of type []L and []R, and applies zip:LxR->M
// elementwise to produce an intermediate array of type []M and length
// equal to the length of the shortest input. This array is then reduced
// with fold expression fold:OxM->O
//
// Equivalent to:
//
//	Reduce(ZipWith(first, second, zip), fold)
//
// Note: the intermediate array is not stored in memory.
//
// Example: compute the inner product (u, v):
//
//	ZipReduce(u, v, func.Mul, func.Add)
func ZipReduce[L, R, M, O any](first []L, second []R, zip func(L, R) M, fold func(O, M) O) O {
	ln := fun.Min(len(first), len(second))

	var o O
	for i := 0; i < ln; i++ {
		o = fold(o, zip(first[i], second[i]))
	}
	return o
}

// AdjacentMap slides a window of size 2 across the array arr applying operator 'f'
// to produce an array of size len(arr)-1.
func AdjacentMap[T, M number](arr []T, f func(T, T) M) []M {
	if len(arr) < 1 {
		return []M{}
	}
	return ZipWith(arr, arr[1:], f)
}

// AdjacentReduce slides a window of size 2 across the array arr applying operator 'zip',
// producing an intermediate array of size len(arr)-1. This array is then reduced with the
// fold operator.
//
// Equivalent to:
//
//	Reduce(AdjacentMap(arr, zip), fold)
//
// Note: the intermediate array is not stored in memory.
func AdjacentReduce[T, A, I any](arr []T, zip func(T, T) I, fold func(A, I) A) A {
	var acc A
	if len(arr) < 2 {
		return acc
	}

	for i := 1; i < len(arr); i++ {
		acc = fold(acc, zip(arr[i-1], arr[i]))
	}
	return acc
}

// Best scans the array and tries to find the "best" entry. An entry arr[i] is the best
// if isBetter(arr[i], arr[j]) is true for any value of j. Expected properties of isBetter:
//
//	Anticommutativity: isBetter(x,y) = !isBetter(y,x)
//	Total ordering:    isBetter(x,y), isBetter(y,z) <==> isBetter(x, z).
//
// Example use: return the maximum value in the list
//
//	Best(arr, func(x, y int) bool { return x>y })
//
// Complexity is O(|arr|).
func Best[T any](arr []T, isBetter fun.Comparator[T]) (acc T) {
	if len(arr) == 0 {
		return acc
	}

	acc = arr[0]
	for _, v := range arr[1:] {
		if isBetter(v, acc) {
			acc = v
		}
	}

	return acc
}

// BestN returns the best N entries in the array arr. An entry arr[i] is the best
// if isBetter(arr[i], arr[j]) is true for any value of j. Expected properties of isBetter:
//
//	Anticommutativity: isBetter(x,y) = !isBetter(y,x)
//	Total ordering:    isBetter(x,y), isBetter(y,z) <=> isBetter(x, z).
//
// Example use: return the 3 largest values in the list
//
//	BestN(arr, 3, func(x, y int) bool { return x>y })
//
// Complexity is O(n·|arr|).
func BestN[T any](arr []T, n uint, isBetter fun.Comparator[T]) []T {
	if uint(len(arr)) <= n {
		n = uint(len(arr))
		acc := make([]T, n)
		copy(acc, arr[:n])
		Sort(acc, isBetter)
		return acc
	}
	acc := make([]T, n)
	copy(acc, arr[:n])
	Sort(acc, isBetter)

	for _, a := range arr[n:] {
		UpdateBestN(n, acc, a, isBetter)
	}

	return acc
}

// UpdateBestN takes a list topN sorted according to isBetter and emplaces
// x in its position. After this, the last element is dropped from the list.
// Note that if x were to be last, the emplace-then-drop step is skipped
// altogether.
//
// Complexity is O(n).
func UpdateBestN[T any](n uint, topN []T, x T, isBetter fun.Comparator[T]) {
	if isBetter(topN[n-1], x) { // Not top n
		return
	}

	i := int(n) - 1
	for ; i > 0; i-- {
		if isBetter(topN[i-1], x) {
			topN[i] = x
			break
		}
		topN[i] = topN[i-1]
	}
	topN[i] = x
}

// Sort sorts a list according to a comparator comp. Item i preceedes
// item j <=> comp(i,j) is true.
//
// Example: sort from smallest to largest:
//
//	Sort(arr, func(l,r int) bool { return l<r }) // Sorts incrementally
//	Sort(arr, fun.Lt)                            // The same, but shorter
//
// Complexity is O(|arr|·log(|arr|)).
func Sort[T any](arr []T, comp fun.Comparator[T]) {
	sort.Slice(arr, func(i, j int) bool {
		return comp(arr[i], arr[j])
	})
}

// Common finds all elements that two slices have in common.
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
func Common[T any](first, second []T, comp fun.Comparator[T]) []T {
	common := []T{}
	var f, s int
	for f < len(first) && s < len(second) {
		// first[f] preceedes second[s]
		if comp(first[f], second[s]) {
			f++
			continue
		}
		// first[f] succeeds second[s]
		if comp(second[s], first[f]) {
			s++
			continue
		}
		// first[f] == second[s]
		common = append(common, first[f])
		f++
		s++
	}
	return common
}

// Unique modifies array arr so that all unique items are moved to the
// beginning. Returns the index where the new end is.
func Unique[T any](arr []T, equal fun.Comparator[T]) (endUnique int) {
	if len(arr) == 0 {
		return 0
	}

	endUnique = 1
	for i := 1; i < len(arr); i++ {
		if equal(arr[i], arr[endUnique-1]) {
			continue
		}
		if i != endUnique {
			arr[endUnique], arr[i] = arr[i], arr[endUnique] // Swap
		}
		endUnique++
	}

	return endUnique
}
