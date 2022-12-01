package main_test

import (
	"fmt"
	"os"
	"testing"

	main "github.com/EduardGomezEscandell/AdventOfCode/2022"
	"github.com/stretchr/testify/require"
)

func TestRun(t *testing.T) {
	testCases := map[string]struct {
		day     uint
		wantErr bool
	}{
		"day0":     {day: 0},
		"day1":     {day: 1},
		"day 1000": {day: 1000, wantErr: true},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			stdout, err := os.CreateTemp(t.TempDir(), "testout")
			require.NoError(t, err, "Failed to create temp file")

			err = main.Run(tc.day, stdout.Name())
			if tc.wantErr {
				require.Error(t, err, "Unexpected success in Start")
				return
			}
			require.NoError(t, err, "Unexpected failure in Start")

			b, err := os.ReadFile(stdout.Name())
			require.NoError(t, err, "Error reading file stdout was redirected to")

			require.Contains(t, string(b), fmt.Sprintf("DAY %d", tc.day))
		})
	}
}
