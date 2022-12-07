// Package tree implements a very basic tree data structure.
package tree

// Tree is the data structure you're thinking of.
type Tree[T any] struct {
	Root Node[T]
}

// Node is the basic unit of the tree. Each node points
// to its data and its children.
type Node[T any] struct {
	Data     *T
	Children []*Node[T]
}

// New creates a tree and its root node.
func New[T any]() Tree[T] {
	return Tree[T]{
		Root: Node[T]{},
	}
}
