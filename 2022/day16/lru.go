package day16

import (
	"container/heap"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/myheap"
)

type lruCache[K comparable, V any] struct {
	byAge    *myheap.Heap[*lruEntry[K, V]]
	byKey    map[K]*lruEntry[K, V]
	data     []lruEntry[K, V]
	capacity int
	epoch    lruEpoch
}

func NewLRUCache[K comparable, V any](cap int) *lruCache[K, V] {
	return &lruCache[K, V]{
		byKey:    map[K]*lruEntry[K, V]{},
		byAge:    myheap.New(func(x, y *lruEntry[K, V]) bool { return x.epoch < y.epoch }),
		data:     make([]lruEntry[K, V], cap),
		capacity: cap,
		epoch:    1,
	}
}

func (lru lruCache[K, V]) Len() int {
	return len(lru.byKey)
}

// Check looks into the cache to see if the given key is
// is registered. If so, the value is returned.
func (lru lruCache[K, V]) Check(key K) (v V, ok bool) {
	entry, ok := lru.byKey[key]
	if !ok {
		return v, false
	}
	v = entry.data
	return
}

// Update checks if an entry was in the cache. If it was,
// it updates its epoch. Otherwise, it adds it anew.
func (lru lruCache[K, V]) Update(k K, v V) {
	lru.epoch++
	entry, ok := lru.byKey[k]
	if ok {
		// log.Printf("Cache hit!")
		entry.epoch = lru.epoch
		return
	}
	var ptr *lruEntry[K, V]
	if lru.Len() >= lru.capacity {
		ptr = heap.Pop(lru.byAge).(*lruEntry[K, V])
		delete(lru.byKey, ptr.key)
	} else {
		ptr = &lru.data[lru.Len()]
	}

	ptr.epoch = lru.epoch
	ptr.key = k
	ptr.data = v

	lru.byKey[k] = ptr
	heap.Push(lru.byAge, ptr)
}

type lruEpoch uint64

type lruEntry[K, V any] struct {
	epoch lruEpoch
	key   K
	data  V
}
