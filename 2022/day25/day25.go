// Package day25 solves day 25 of AoC2022.
package day25

import (
	"bufio"
	"bytes"
	"fmt"
	"io"
	"math"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
	"github.com/EduardGomezEscandell/algo/algo"
	"github.com/EduardGomezEscandell/algo/utils"
)

const (
	today    = 25
	fileName = "input.txt"
)

// Part1 solves the first half of today's problem.
func Part1(data []string) (string, error) {
	num := algo.MapReduce(data, snafuDecode, utils.Add[int], 0)
	return snafuEncode(num), nil
}

// Part2 solves the second half of today's problem.
func Part2([]string) (string, error) {
	return "Merry Christmas!", nil
}

// ------------ Implementation ---------------------

var digitDecoder = map[byte]int{
	'2': 2,
	'1': 1,
	'0': 0,
	'-': -1,
	'=': -2,
}

func snafuDecode(s string) int {
	base := 1
	var num int
	for i := len(s) - 1; i >= 0; i-- {
		digit, ok := digitDecoder[s[i]]
		if !ok {
			panic(fmt.Errorf("unexpected character '%c' in SNAFU %q", s[i], s))
		}
		num += base * digit
		base *= 5
	}
	return num
}

func snafuEncode(num int) string {
	maxLen := log5(num) + 2 // +1 for integer rounding and +1 to make room for a possible (1-) or (1=)
	snafu := algo.Generate(maxLen, func() rune { return '0' })

	for idx := len(snafu) - 1; num > 0; idx-- {
		d := num % 5
		num /= 5

		switch d {
		case 0:
			snafu[idx] = '0'
		case 1:
			snafu[idx] = '1'
		case 2:
			snafu[idx] = '2'
		case 3:
			snafu[idx] = '='
			num++
		case 4:
			snafu[idx] = '-'
			num++
		}
	}

	// Removing leading zeros
	begin := algo.FindIf(snafu, func(r rune) bool { return r != '0' })
	return string(snafu[begin:])
}

func log5(n int) int {
	const log5 = 2.32192809488736 // log2(5)
	return int(math.Log2(float64(n)) / log5)
}

// ---------- Here be boilerplate ------------------

// Main is the entry point to today's problem's solution.
func Main(stdout io.Writer) error {
	data, err := ReadData()
	if err != nil {
		return err
	}

	p1, err := Part1(data)
	if err != nil {
		return fmt.Errorf("error in part 1: %v", err)
	}
	fmt.Fprintf(stdout, "Result of part 1: %s\n", p1)

	p2, err := Part2(data)
	if err != nil {
		return fmt.Errorf("error in part 2: %v", err)
	}
	fmt.Fprintf(stdout, "Result of part 2: %s\n", p2)

	return nil
}

// ReadDataFile is a wrapper around input.ReadDataFile made to be
// easily mocked.
var ReadDataFile = func() ([]byte, error) {
	return input.ReadDataFile(today, fileName)
}

// ReadData reads the data file.
func ReadData() (data []string, err error) {
	b, err := ReadDataFile()
	if err != nil {
		return nil, err
	}

	sc := bufio.NewScanner(bytes.NewReader(b))

	data = []string{}
	for row := 0; sc.Scan(); row++ {
		data = append(data, sc.Text())
	}

	return data, sc.Err()
}
