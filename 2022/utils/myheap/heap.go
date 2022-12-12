// Package myheap implemements a heap data structure.
package myheap

import (
	"container/heap"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
)

// A Heap is the heap data structure.
type Heap[T any] struct {
	data []T
	comp func(x, y T) bool
}

// New creates and initializes a heap.
func New[T any](best fun.Comparator[T]) *Heap[T] {
	h := &Heap[T]{
		data: []T{},
		comp: best,
	}
	heap.Init(h)
	return h
}

func (h Heap[T]) Len() int           { return len(h.data) }
func (h Heap[T]) Less(i, j int) bool { return h.comp(h.data[i], h.data[j]) }
func (h Heap[T]) Swap(i, j int)      { h.data[i], h.data[j] = h.data[j], h.data[i] }

// Push emplaces the leading item. DO NOT USE!
// Use heap.Push(h, x) instead.
func (h *Heap[T]) Push(x any) {
	h.data = append(h.data, x.(T)) // nolint: forcetypeassert
	// We simply allow a panic ^.
}

// Pop extracts the leading item. DO NOT USE!
// Use heap.Pop(h).(T) instead.
func (h *Heap[T]) Pop() any {
	old := h.data
	n := len(old)
	x := old[n-1]
	h.data = old[:n-1]
	return x
}
