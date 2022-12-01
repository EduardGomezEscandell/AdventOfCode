package input_test

import (
	"os"
	"path/filepath"
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
	"github.com/stretchr/testify/require"
)

func TestFilename(t *testing.T) {
	cwd, err := os.Getwd()
	require.NoError(t, err)

	testCases := map[string]struct {
		root     string
		day      uint
		filename string
		want     string
		wantErr  bool
	}{
		"one digit day": {root: "/tmp/", day: 1, filename: "data.txt", want: "/tmp/data/01/data.txt"},
		"two digit day": {root: "/tmp/dir/", day: 13, filename: "things.csv", want: "/tmp/dir/data/13/things.csv"},
		"no filename":   {root: "/tmp/dir/", day: 5, wantErr: true},
		"no env":        {day: 5, filename: "data.tar.gz", want: filepath.Join(cwd, "data/05/data.tar.gz")},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			os.Setenv(input.EnvRootDir, tc.root)

			got, err := input.DataFullPath(tc.day, tc.filename)

			if tc.wantErr {
				require.Error(t, err)
				return
			}
			require.NoError(t, err)

			require.Equal(t, tc.want, got)
		})
	}
}
