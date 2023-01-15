package day25_test

import (
	"bytes"
	"log"
	"os"
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day25"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/testutils"
	"github.com/stretchr/testify/require"
)

func TestMain(m *testing.M) {
	err := testutils.CheckEnv()
	if err != nil {
		log.Printf("Setup: %v", err)
	}
	r := m.Run()
	os.Exit(r)
}

func TestPart1Part2(t *testing.T) { // nolint: dupl
	testCases := map[string]struct {
		data []string
		want string
	}{
		"example": {
			data: []string{},
			want: "Hello, world!",
		},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			got, err := day25.Part1(tc.data)
			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: Hello, world!
Result of part 2: Hello, world!
`
	buff := new(bytes.Buffer)

	err := day25.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
