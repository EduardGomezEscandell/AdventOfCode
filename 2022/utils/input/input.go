// Package input deals with reading from the data folder.
package input

import (
	"bufio"
	"context"
	"errors"
	"fmt"
	"io"
	"log"
	"os"
	"path/filepath"
)

const (
	// EnvRootDir is the environment variable indicating where the 2022 AoC project root is.
	EnvRootDir = "AOC_2022_ROOT"
)

// DataFullPath provides the full path to the requested data file.
// It uses AOC_2022_ROOT to know where the 2022 AoC project root is.
func DataFullPath(day uint, filename string) (string, error) {
	if len(filename) == 0 {
		return "", errors.New("filename cannot be empty")
	}
	root := os.Getenv(EnvRootDir)
	if root == "" {
		var err error
		root, err = os.Getwd()
		if err != nil {
			log.Printf("Failed call to os.Getwd() to get project root: %v", err)
			root = "."
		}
	}
	return filepath.Join(root, "data", fmt.Sprintf("%.02d", day), filename), nil
}

// ReadDataFile reads the requested data file.
func ReadDataFile(day uint, filename string) ([]byte, error) {
	path, err := DataFullPath(day, filename)
	if err != nil {
		return []byte{}, err
	}
	return os.ReadFile(path) // nolint: gosec
}

// DataFile opens the requested data file.
func DataFile(day uint, filename string) (*os.File, error) {
	path, err := DataFullPath(day, filename)
	if err != nil {
		return nil, err
	}
	return os.Open(path)
}

// Line packs info relevant to reading a line from a file.
type Line struct {
	string
	error
}

// NewLine creates a mew Line.
func NewLine(s string) Line {
	return Line{string: s}
}

// Str reveals the string.
func (l Line) Str() string {
	return l.string
}

// Err reveals the error.
func (l Line) Err() error {
	return l.error
}

// ReadDataAsync returns a channel that can read the input file line by line,
// asyncronously. lookAhead is the number of lines that the reader can pre-fetch
// from the file.
func ReadDataAsync(ctx context.Context, reader io.ReadCloser, lookAhead int) (<-chan Line, error) {
	ch := make(chan Line, lookAhead)

	go func() {
		defer reader.Close()
		defer close(ch)

		sc := bufio.NewScanner(reader)
		for sc.Scan() {
			ln := NewLine(sc.Text())

			select {
			case <-ctx.Done():
				ch <- Line{error: ctx.Err()}
				return
			case ch <- ln:
			}
		}

		if sc.Err() != nil {
			ch <- Line{error: ctx.Err()}
		}
	}()

	return ch, nil
}
