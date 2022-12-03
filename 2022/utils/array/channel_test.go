package array_test

import (
	"context"
	"testing"
	"time"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/channel"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/generics"
	"github.com/stretchr/testify/require"
)

func TestFromChannel(t *testing.T) {
	t.Parallel()
	t.Run("int", testFromChannel[int])
	t.Run("int8", testFromChannel[int8])
	t.Run("int32", testFromChannel[int32])
	t.Run("int64", testFromChannel[int64])
}
func testFromChannel[T generics.Signed](t *testing.T) { // nolint: thelper
	t.Parallel()
	testCases := map[string]struct {
		data     []T
		maxLen   int
		capacity int // Should not affect results
		buffer   int
	}{
		"empty":                      {data: []T{}, maxLen: -1, capacity: -1},
		"empty, extra capacity":      {data: []T{}, maxLen: -1, capacity: 16},
		"don't read":                 {data: []T{1, 35, -8}, maxLen: 0, capacity: -1},
		"don't read, extra capacity": {data: []T{1, 35, -8}, maxLen: 0, capacity: 16},
		"read some":                  {data: []T{0, 64, -8, 118, 99}, maxLen: 3, capacity: -1},
		"read some, extra capacity":  {data: []T{0, 64, -8, 118, 99}, maxLen: 3, capacity: 16},
		"read all":                   {data: []T{0, 64, -8, 118, 99}, maxLen: -1, capacity: -1},
		"read all, extra capacity":   {data: []T{0, 64, -8, 118, 99}, maxLen: -1, capacity: 16},
		"read all, extra MaxLen":     {data: []T{0, 64, -8, 118, 99}, maxLen: 2 << 61, capacity: -1},
		// Large number to crash any machine if it is misused as preallocation^^^^^^^
		"read all, buffered": {data: []T{0, 64, -8, 118, 99}, maxLen: -1, capacity: -1, buffer: 2},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			input := make([]T, len(tc.data))
			copy(input, tc.data)

			ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
			defer cancel()

			ch := channel.FromArray(ctx, input, tc.buffer)

			var got []T
			if tc.maxLen >= 0 {
				got = array.FromChannel(ch, array.MaxLen(tc.maxLen))
				wantLen := fun.Min(tc.maxLen, len(tc.data))
				require.Equalf(t, wantLen, len(got),
					"Unexpected length. Should be equal to Min(len(data), maxLen): Min(%d, %d) => %d", len(tc.data), tc.maxLen, wantLen)
				require.Equal(t, tc.data[:wantLen], got)
				return
			}
			got = array.FromChannel(ch)
			wantLen := len(tc.data)
			require.Equalf(t, wantLen, len(got), "Unexpected length. Should be equal to len(data) => %d", wantLen)
			require.Equal(t, tc.data, got)
		})
	}
}
