// Package testutils offers utilities to help in tests.
package testutils

import (
	"fmt"
	"os"
	"reflect"
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
	"github.com/stretchr/testify/require"
)

// Backup the specified list of elements before further mocking.
//
// from snapcore/snapd.
func Backup(mockablesByPtr ...interface{}) (restore func()) {
	backup := backupMockables(mockablesByPtr)

	return func() {
		for i, ptr := range mockablesByPtr {
			mockedPtr := reflect.ValueOf(ptr)
			mockedPtr.Elem().Set(backup[i].Elem())
		}
	}
}

// from snapcore/snapd.
func backupMockables(mockablesByPtr []interface{}) (backup []*reflect.Value) {
	backup = make([]*reflect.Value, len(mockablesByPtr))

	for i, ptr := range mockablesByPtr {
		mockedPtr := reflect.ValueOf(ptr)

		if mockedPtr.Type().Kind() != reflect.Ptr {
			panic("Backup: each mockable must be passed by pointer!")
		}

		saved := reflect.New(mockedPtr.Elem().Type())
		saved.Elem().Set(mockedPtr.Elem())
		backup[i] = &saved
	}
	return backup
}

// RequireCheckEnv ensures that the necessary environment variables are set.
func RequireCheckEnv(t *testing.T) {
	t.Helper()
	require.NoError(t, CheckEnv(), "Setup: Missing environment variables.")
}

// CheckEnv errors out if the necessary environment variables are set.
func CheckEnv() error {
	env := os.Getenv(input.EnvRootDir)
	if env == "" {
		return fmt.Errorf("Environment variable %s not set. It should point to the root of the 2022 AoC project", input.EnvRootDir)
	}
	return nil
}
