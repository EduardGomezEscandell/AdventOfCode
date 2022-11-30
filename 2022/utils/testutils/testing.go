// Package testutils offers utilities to help in tests.
package testutils

import (
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

func CheckEnv(t *testing.T) {
	t.Helper()
	env := os.Getenv(input.EnvRootDir)
	require.NotEmpty(t, env, "Environment variable %s not set. It should point to the root of the 2022 AoC project", input.EnvRootDir)
}
