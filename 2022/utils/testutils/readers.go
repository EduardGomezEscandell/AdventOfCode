package testutils

import (
	"strings"
	"sync/atomic"
	"testing"

	"github.com/stretchr/testify/require"
)

// MockReadCloser mocks an io.ReadCloser with a predefined string
// and makes some checks to ensure it is used properly.
type MockReadCloser struct {
	*strings.Reader
	t      *testing.T
	closed atomic.Value
}

// NewMockReadCloser creates a new instance of *MockReadCloser.
func NewMockReadCloser(t *testing.T, contents string) *MockReadCloser {
	t.Helper()
	r := strings.NewReader(contents)
	rc := MockReadCloser{
		Reader: r,
		t:      t,
	}
	rc.closed.Store(false)
	t.Cleanup(func() {
		closed := rc.closed.Load().(bool) // nolint: forcetypeassert
		require.True(t, closed, "Failed to close string read closer with contents:\n%s", contents)
	})
	return &rc
}

// Close implements the io.Closer interface. It asserts
// it was not called twice.
func (rc *MockReadCloser) Close() error {
	rc.t.Helper()
	require.False(rc.t, rc.closed.Load().(bool), "Closed twice!") // nolint: forcetypeassert
	rc.closed.Store(true)
	return nil
}

func (rc *MockReadCloser) Read(b []byte) (int, error) {
	rc.t.Helper()
	require.False(rc.t, rc.closed.Load().(bool), "Read after close") // nolint: forcetypeassert
	return rc.Reader.Read(b)
}
