package channel_test

import (
	"context"
	"testing"
	"time"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/channel"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/charray"
	"github.com/EduardGomezEscandell/algo/algo"
	"github.com/EduardGomezEscandell/algo/utils"
	"github.com/stretchr/testify/require"
)

func TestSplit(t *testing.T) {
	t.Parallel()
	t.Run("int", testSplit[int])
	t.Run("int8", testSplit[int8])
	t.Run("int32", testSplit[int32])
	t.Run("int64", testSplit[int64])
}

func testSplit[T utils.Signed](t *testing.T) { // nolint: thelper
	t.Parallel()
	testCases := map[string]struct {
		data      []T
		nChannels int
		buffer    int
	}{
		"empty":         {},
		"happy path":    {data: []T{1, 2, 8, 16, 99, 101}, nChannels: 2},
		"no outputs":    {data: []T{1, 2, 8, 16, 99, 101}, nChannels: 0},
		"single":        {data: []T{9, 77, 5, 99, -15}, nChannels: 1},
		"three readers": {data: []T{9, 15, 23, 9, 17}, nChannels: 3},
		"buffered":      {data: []T{9, 15, 23, 9, 17}, nChannels: 3, buffer: 2},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			ctx, cancel := context.WithTimeout(context.Background(), time.Second)
			defer cancel()
			input := charray.FromArray(ctx, tc.data, tc.buffer)

			chans := channel.Split(ctx, input, tc.nChannels)
			require.Equal(t, tc.nChannels, len(chans))

			for i, want := range tc.data {
				received := algo.Map(chans, channel.RecvOk[T])
				for chID, recv := range received {
					require.True(t, recv.Ok, "Unexpectedly closed channel #%d after iteration #%d", chID, i)
					require.Equalf(t, want, recv.V, "Mismatch between expected values for channel #%d after iteration #%d", chID, i)
				}
			}
		})
	}
}
