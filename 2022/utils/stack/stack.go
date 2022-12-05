package stack

import (
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
)

type Stack[T any] struct {
	data []T
}

func New[T any](args ...int) Stack[T] {
	var size, capacity int
	switch len(args) {
	default:
		panic("Only two args allowed: size and capacity")
	case 2:
		capacity = args[1]
		fallthrough
	case 1:
		size = args[0]
		fallthrough
	case 0:
		return Stack[T]{data: make([]T, size, capacity)}
	}
}

func (s Stack[T]) Size() int {
	return len(s.data)
}

func (s Stack[T]) IsEmpty() bool {
	return s.Size() == 0
}

func (s Stack[T]) Peek() T {
	if s.IsEmpty() {
		panic("peeking into empty stack")
	}
	return s.data[len(s.data)-1]
}

func (s *Stack[T]) Pop() {
	if s.IsEmpty() {
		panic("peeking into empty stack")
	}
	s.data = s.data[:len(s.data)-1]
}

func (s *Stack[T]) Push(t T) {
	s.data = append(s.data, t)
}

func (s *Stack[T]) Data() []T {
	return s.data
}

func (s *Stack[T]) Invert() {
	s.data = array.Reverse(s.data)
}
