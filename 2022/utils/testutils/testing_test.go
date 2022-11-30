package testutils_test

import (
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/testutils"
	"github.com/stretchr/testify/require"
)

func TestMock(t *testing.T) {
	t.Parallel()

	testCases := map[string]struct {
		mocked interface{}
		value  interface{}
	}{
		"integer": {mocked: int(63), value: int(74)},
		"string":  {mocked: "hello", value: "goodbye"},
		"pointer": {mocked: new(uint), value: nil},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			v := tc.mocked

			restore := testutils.Backup(&v)
			defer restore()

			v = tc.value
			require.Equal(t, v, tc.value)

			restore()
			require.Equal(t, v, tc.mocked)

			restore() // Ensuring no panics
			require.Equal(t, v, tc.mocked)

		})
	}
}

func TestMockFunc(t *testing.T) {
	t.Parallel()

	orig := func() int { return 0 }
	mock := func() int { return 42 }

	f := orig
	require.Equal(t, f(), orig())

	restore := testutils.Backup(&f)
	defer restore()

	f = mock
	require.Equal(t, f(), mock())

	restore()
	require.Equal(t, f(), orig())

	restore()
	require.Equal(t, f(), orig())
}
