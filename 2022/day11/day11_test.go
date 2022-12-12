package day11_test

import (
	"bytes"
	"log"
	"os"
	"strings"
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day11"
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

func TestReadData(t *testing.T) {
	testCases := map[string]struct {
		data []string
		want []day11.Monkey
	}{
		"single":  {data: example[:6], want: exampleMonkeys[:1]},
		"two":     {data: example[:13], want: exampleMonkeys[:2]},
		"example": {data: example, want: exampleMonkeys},
		"data":    {data: realData, want: realMonkeys},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			defer testutils.Backup(&day11.ReadDataFile)()
			day11.ReadDataFile = func() ([]byte, error) {
				return []byte(strings.Join(tc.data, "\n")), nil
			}

			got, err := day11.ParseInput()
			require.NoError(t, err)

			require.Equal(t, len(tc.want), len(got))
			for i := range got {
				requireEqualMonkeys(t, tc.want[i], got[i])
			}
		})
	}
}

func TestPart1(t *testing.T) {
	t.Parallel()

	testCases := map[string]struct {
		data []day11.Monkey
		want uint64
	}{
		"example": {data: exampleMonkeys, want: 10605},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			monkeys := make([]day11.Monkey, len(tc.data))
			copy(monkeys, tc.data)

			got, err := day11.Part1(monkeys)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestSolve(t *testing.T) {
	t.Parallel()

	testCases := map[string]struct {
		data    []day11.Monkey
		nrounds int
		want    uint64
	}{
		"example 1":   {data: exampleMonkeys, nrounds: 1, want: 6 * 4},
		"example 20":  {data: exampleMonkeys, nrounds: 20, want: 103 * 99},
		"example 1k":  {data: exampleMonkeys, nrounds: 1_000, want: 5204 * 5192},
		"example 5k":  {data: exampleMonkeys, nrounds: 5_000, want: 26075 * 26000},
		"example 9k":  {data: exampleMonkeys, nrounds: 9_000, want: 46945 * 46807},
		"example 10k": {data: exampleMonkeys, nrounds: 10_000, want: 52166 * 52013},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			monkeys := make([]day11.Monkey, len(tc.data))
			copy(monkeys, tc.data)

			got, err := day11.Solve(monkeys, tc.nrounds, 1)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart2(t *testing.T) {
	t.Parallel()

	testCases := map[string]struct {
		data []day11.Monkey
		want uint64
	}{
		"example": {data: exampleMonkeys, want: 2_713_310_158},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			monkeys := make([]day11.Monkey, len(tc.data))
			copy(monkeys, tc.data)

			got, err := day11.Part2(monkeys)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: 120056
Result of part 2: 21816744824
`
	buff := new(bytes.Buffer)

	err := day11.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}

func requireEqualMonkeys(t *testing.T, m1, m2 day11.Monkey) {
	t.Helper()

	require.Equalf(t, m1.ID, m2.ID, "Mismatching monkey ids:\n%v\n%v", m1, m2)
	require.Equalf(t, m1.Inventory, m2.Inventory, "Mismatching monkey inventories:\n%v\n%v", m1, m2)
	require.Equalf(t, m1.TestValue, m2.TestValue, "Mismatching monkey test values:\n%v\n%v", m1, m2)
	require.Equalf(t, m1.ThrowTrue, m2.ThrowTrue, "Mismatching monkey throw true:\n%v\n%v", m1, m2)
	require.Equalf(t, m1.ThrowFalse, m2.ThrowFalse, "Mismatching monkey throw false:\n%v\n%v", m1, m2)
	require.Equalf(t, m1.A, m2.A, "Mismatching monkey inspection A:\n%v\n%v", m1, m2)
	require.Equalf(t, m1.B, m2.B, "Mismatching monkey inspection B:\n%v\n%v", m1, m2)
	require.Equalf(t, m1.C, m2.C, "Mismatching monkey inspection C:\n%v\n%v", m1, m2)
}

var example = []string{
	"Monkey 0:",
	"  Starting items: 79, 98",
	"  Operation: new = old * 19",
	"  Test: divisible by 23",
	"    If true: throw to monkey 2",
	"    If false: throw to monkey 3",
	"",
	"Monkey 1:",
	"  Starting items: 54, 65, 75, 74",
	"  Operation: new = old + 6",
	"  Test: divisible by 19",
	"    If true: throw to monkey 2",
	"    If false: throw to monkey 0",
	"",
	"Monkey 2:",
	"  Starting items: 79, 60, 97",
	"  Operation: new = old * old",
	"  Test: divisible by 13",
	"    If true: throw to monkey 1",
	"    If false: throw to monkey 3",
	"",
	"Monkey 3:",
	"  Starting items: 74",
	"  Operation: new = old + 3",
	"  Test: divisible by 17",
	"    If true: throw to monkey 0",
	"    If false: throw to monkey 1",
}

var exampleMonkeys = []day11.Monkey{
	{
		ID:         0,
		Inventory:  []uint64{79, 98},
		A:          0,
		B:          19,
		C:          0,
		TestValue:  23,
		ThrowTrue:  2,
		ThrowFalse: 3,
	},
	{
		ID:         1,
		Inventory:  []uint64{54, 65, 75, 74},
		A:          0,
		B:          1,
		C:          6,
		TestValue:  19,
		ThrowTrue:  2,
		ThrowFalse: 0,
	},
	{
		ID:         2,
		Inventory:  []uint64{79, 60, 97},
		A:          1,
		B:          0,
		C:          0,
		TestValue:  13,
		ThrowTrue:  1,
		ThrowFalse: 3,
	},
	{
		ID:         3,
		Inventory:  []uint64{74},
		A:          0,
		B:          1,
		C:          3,
		TestValue:  17,
		ThrowTrue:  0,
		ThrowFalse: 1,
	},
}

var realData = []string{
	"Monkey 0:",
	"  Starting items: 89, 74",
	"  Operation: new = old * 5",
	"  Test: divisible by 17",
	"    If true: throw to monkey 4",
	"    If false: throw to monkey 7",
	"",
	"Monkey 1:",
	"  Starting items: 75, 69, 87, 57, 84, 90, 66, 50",
	"  Operation: new = old + 3",
	"  Test: divisible by 7",
	"    If true: throw to monkey 3",
	"    If false: throw to monkey 2",
	"",
	"Monkey 2:",
	"  Starting items: 55",
	"  Operation: new = old + 7",
	"  Test: divisible by 13",
	"    If true: throw to monkey 0",
	"    If false: throw to monkey 7",
	"",
	"Monkey 3:",
	"  Starting items: 69, 82, 69, 56, 68",
	"  Operation: new = old + 5",
	"  Test: divisible by 2",
	"    If true: throw to monkey 0",
	"    If false: throw to monkey 2",
	"",
	"Monkey 4:",
	"  Starting items: 72, 97, 50",
	"  Operation: new = old + 2",
	"  Test: divisible by 19",
	"    If true: throw to monkey 6",
	"    If false: throw to monkey 5",
	"",
	"Monkey 5:",
	"  Starting items: 90, 84, 56, 92, 91, 91",
	"  Operation: new = old * 19",
	"  Test: divisible by 3",
	"    If true: throw to monkey 6",
	"    If false: throw to monkey 1",
	"",
	"Monkey 6:",
	"  Starting items: 63, 93, 55, 53",
	"  Operation: new = old * old",
	"  Test: divisible by 5",
	"    If true: throw to monkey 3",
	"    If false: throw to monkey 1",
	"",
	"Monkey 7:",
	"  Starting items: 50, 61, 52, 58, 86, 68, 97",
	"  Operation: new = old + 4",
	"  Test: divisible by 11",
	"    If true: throw to monkey 5",
	"    If false: throw to monkey 4",
}

var realMonkeys = []day11.Monkey{
	{
		ID:         0,
		Inventory:  []uint64{89, 74},
		A:          0,
		B:          5,
		C:          0,
		TestValue:  17,
		ThrowTrue:  4,
		ThrowFalse: 7,
	},
	{
		ID:         1,
		Inventory:  []uint64{75, 69, 87, 57, 84, 90, 66, 50},
		A:          0,
		B:          1,
		C:          3,
		TestValue:  7,
		ThrowTrue:  3,
		ThrowFalse: 2,
	},
	{
		ID:         2,
		Inventory:  []uint64{55},
		A:          0,
		B:          1,
		C:          7,
		TestValue:  13,
		ThrowTrue:  0,
		ThrowFalse: 7,
	},
	{
		ID:         3,
		Inventory:  []uint64{69, 82, 69, 56, 68},
		A:          0,
		B:          1,
		C:          5,
		TestValue:  2,
		ThrowTrue:  0,
		ThrowFalse: 2,
	},
	{
		ID:         4,
		Inventory:  []uint64{72, 97, 50},
		A:          0,
		B:          1,
		C:          2,
		TestValue:  19,
		ThrowTrue:  6,
		ThrowFalse: 5,
	},
	{
		ID:         5,
		Inventory:  []uint64{90, 84, 56, 92, 91, 91},
		A:          0,
		B:          19,
		C:          0,
		TestValue:  3,
		ThrowTrue:  6,
		ThrowFalse: 1,
	},
	{
		ID:         6,
		Inventory:  []uint64{63, 93, 55, 53},
		A:          1,
		B:          0,
		C:          0,
		TestValue:  5,
		ThrowTrue:  3,
		ThrowFalse: 1,
	},
	{
		ID:         7,
		Inventory:  []uint64{50, 61, 52, 58, 86, 68, 97},
		A:          0,
		B:          1,
		C:          4,
		TestValue:  11,
		ThrowTrue:  5,
		ThrowFalse: 4,
	},
}
