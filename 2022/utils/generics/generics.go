// Package generics contains constraint definitions
package generics

import "golang.org/x/exp/constraints"

// Number constrains numeric values.
type Number interface {
	constraints.Integer | constraints.Float
}

// Signed constrains to Numbers where negative values can be represented.
type Signed interface {
	constraints.Signed | constraints.Float
}
