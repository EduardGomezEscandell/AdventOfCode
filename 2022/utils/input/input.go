// Package input deals with reading from the data folder.
package input

import (
	"errors"
	"fmt"
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
