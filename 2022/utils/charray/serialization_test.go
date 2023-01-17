package charray_test

import (
	"context"
	"testing"
	"time"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/charray"
	"github.com/EduardGomezEscandell/algo/utils"
	"github.com/stretchr/testify/require"
)

func TestSerialize(t *testing.T) {
	t.Parallel()
	t.Run("int", testSerialize[int])
	t.Run("int8", testSerialize[int8])
	t.Run("int32", testSerialize[int32])
	t.Run("int64", testSerialize[int64])
}

func TestDeserialize(t *testing.T) {
	t.Parallel()
	t.Run("int", testDeserialize[int])
	t.Run("int8", testDeserialize[int8])
	t.Run("int32", testDeserialize[int32])
	t.Run("int64", testDeserialize[int64])
}

func testSerialize[T utils.Signed](t *testing.T) { // nolint: thelper
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

			ch := charray.Serialize(ctx, input, tc.capacity)

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

func testDeserialize[T utils.Signed](t *testing.T) { // nolint: thelper
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

			ctx, cancel := context.WithTimeout(context.Background(), time.Second)
			defer cancel()

			ch := charray.Serialize(ctx, input, tc.buffer)

			var got []T
			if tc.maxLen >= 0 {
				got = charray.Deserialize(ch, charray.MaxLen(tc.maxLen))
				wantLen := utils.Min(tc.maxLen, len(tc.data))
				require.Equalf(t, wantLen, len(got),
					"Unexpected length. Should be equal to Min(len(data), maxLen): Min(%d, %d) => %d", len(tc.data), tc.maxLen, wantLen)
				require.Equal(t, tc.data[:wantLen], got)
				return
			}
			got = charray.Deserialize(ch)
			wantLen := len(tc.data)
			require.Equalf(t, wantLen, len(got), "Unexpected length. Should be equal to len(data) => %d", wantLen)
			require.Equal(t, tc.data, got)
		})
	}
}
