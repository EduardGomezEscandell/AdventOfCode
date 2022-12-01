package main_test

import (
	"os"
	"testing"

	main "github.com/EduardGomezEscandell/AdventOfCode/2022"
	"github.com/stretchr/testify/require"
)

func TestDay00(t *testing.T) {
	testCases := map[string]struct {
		day      uint
		expected string
		wantErr  bool
	}{
		"day0":     {day: 0, expected: "Result of part 1: 1715\nResult of part 2: 1739\n"},
		"day 1000": {day: 1000, wantErr: true},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			stdout, err := os.CreateTemp(t.TempDir(), "testout")
			require.NoError(t, err, "Failed to create temp file")

			err = main.Start(tc.day, stdout.Name())
			if tc.wantErr {
				require.Error(t, err, "Unexpected success in Start")
				return
			}
			require.NoError(t, err, "Unexpected failure in Start")

			b, err := os.ReadFile(stdout.Name())
			require.NoError(t, err, "Error reading file stdout was redirected to")

			require.Equal(t, tc.expected, string(b))
		})
	}
}
