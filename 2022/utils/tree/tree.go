package tree

type Tree[T any] struct {
	Root Node[T]
}

type Node[T any] struct {
	Value    T
	Children []*Node[T]
}

func New[T any]() Tree[T] {
	return Tree[T]{
		Root: Node[T]{},
	}
}
