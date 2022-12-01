package fun

// Curry1 curries a unary function.
//
// It takes a function f(x) and a value X, and returns function g() := f(X).
func Curry1[I, O any](f func(I) O, input I) func() O {
	return func() O {
		return f(input)
	}
}

// Curry2R curries the (R)ight argument of a binary function.
//
// It takes a function f(x,y) and a value Y, returns function g(x) := f(x, Y).
func Curry2R[L, R, O any](f func(L, R) O, right R) func(L) O {
	return func(l L) O {
		return f(l, right)
	}
}

// Curry2L curries the (L)eft argument of a binary function.
//
// Curry2L takes a value X and a function f(x,y), returns function g(y) := f(X, y).
func Curry2L[L, R, O any](left L, f func(L, R) O) func(R) O {
	return func(r R) O {
		return f(left, r)
	}
}
