package charray_test

import (
	"context"
	"testing"
	"time"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/charray"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/generics"
	"github.com/stretchr/testify/require"
)

func TestFromArray(t *testing.T) {
	t.Parallel()
	t.Run("int", testFromArray[int])
	t.Run("int8", testFromArray[int8])
	t.Run("int32", testFromArray[int32])
	t.Run("int64", testFromArray[int64])
}

func testFromArray[T generics.Signed](t *testing.T) { // nolint: thelper
	t.Parallel()

	const (
		End = iota
		Middle
		Start
	)

	testCases := map[string]struct {
		whenCancel int
		data       []T
		capacity   int
	}{
		"empty":       {data: []T{}, whenCancel: End},
		"happy path":  {data: []T{1, 3, 5, 9}, whenCancel: End},
		"precanceled": {data: []T{1, 1, 6, 8, 7, 7, -64}, whenCancel: Start},
		"canceled":    {data: []T{3, -1, 8, 15}, whenCancel: Middle},
		"buffered":    {data: []T{1, 3, 5, 9}, whenCancel: End, capacity: 2},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			ctx, cancel := context.WithTimeout(context.Background(), time.Second)
			defer cancel()

			input := make([]T, len(tc.data))
			copy(input, tc.data)

			ch := charray.FromArray(ctx, input, tc.capacity)

			// Stopping before starting?
			if tc.whenCancel == Start {
				cancel()
				d, ok := <-ch
				require.False(t, ok, "failed to close channel before starting: read %d", d)
				return
			}

			// Starting
			until := len(tc.data) / 2
			for i := 0; i < until; i++ {
				d, ok := <-ch
				require.True(t, ok, "channel closed too early: first half")
				require.Equalf(t, tc.data[i], d, "Mismatch first half: entry #%d", i)
			}

			// Stopping halfway through?
			if tc.whenCancel == Middle {
				cancel()
				_, ok := <-ch
				require.False(t, ok, "failed to close channel halfway through consuming the array (next idx=%d)", until)
				return
			}

			// Finishing
			for i := until; i < len(tc.data); i++ {
				d, ok := <-ch
				require.True(t, ok, "channel closed too early: second half")
				require.Equalf(t, tc.data[i], d, "Mismatch second half: entry #%d", i)
			}

			// Stopping after finishing
			// Should auto-cancel when the entire array has been read
			_, ok := <-ch
			require.False(t, ok, "failed to close channel after consuming the entire array")

			require.Equal(t, tc.data, input, "channel.FromArray has modified the intput array when it shouldn't've.")
		})
	}
}
