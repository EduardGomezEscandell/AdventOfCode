#ifndef HASH_TABLE_INCLUDED_H
#define HASH_TABLE_INCLUDED_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "vector.h"

typedef size_t ht_hash_t;
typedef int ht_key_t;
typedef int ht_val_t;

typedef struct
{
	ht_key_t key;
	ht_val_t value;
} HT_Pair;

TEMPLATE_VECTOR(HT_Pair*) HT_Bucket;
TEMPLATE_VECTOR(HT_Bucket) HT_BuckeVector;
TEMPLATE_VECTOR(HT_Pair) HT_PairVector;	

typedef struct 
{
	HT_BuckeVector buckets;
	HT_PairVector data;
	ht_hash_t (*hash_function)(const ht_key_t * const key, const size_t n_buckets);
} HashTable;

HT_Bucket NewBucket()
{
	HT_Bucket bucket;
	bucket.capacity = 1;
    bucket.begin = malloc(bucket.capacity * sizeof(*bucket.begin));
    bucket.end = bucket.begin; 
    return bucket;
}

HashTable NewHashTable(ht_hash_t (*hash_function)(const ht_key_t * const key, const size_t n_buckets))
{
	HashTable ht;
	
	NEW_VECTOR(ht.buckets);
	NEW_VECTOR(ht.data);

	const size_t n_buckets = 8;
	RESERVE(ht.buckets, n_buckets);
	ht.buckets.end += n_buckets;

	for(HT_Bucket * it = ht.buckets.begin; it != ht.buckets.end; ++it)
		*it = NewBucket();

	ht.hash_function = hash_function;

	return ht;
}

void ClearHashTable(HashTable * ht)
{
	for(HT_Bucket * it = ht->buckets.begin; it != ht->buckets.end; ++it)
		CLEAR(*it);
	CLEAR(ht->buckets);
	CLEAR(ht->data);
	ht->hash_function = NULL;
}

void PrintHashTable(HashTable * const ht, const char * key_format, const char * value_format)
{
	printf("{\n");
	for(HT_Bucket const * b = ht->buckets.begin; b != ht->buckets.end; ++b)
	{
		for(HT_Pair * const * it = b->begin; it != b->end; ++it)
		{
			printf("    ");
			printf(key_format, (*it)->key);
			printf(" : ");
			printf(value_format, (*it)->value);
			printf("\n");
		}
	}
	printf("}\n");
}

ht_hash_t HashIntegers(const ht_key_t * const key, const size_t n_buckets)
{
	const ht_hash_t hash = (*key) * (*key);
	return hash % n_buckets;
}

typedef struct
{
	HT_Bucket * bucket;
	HT_Pair * pair;
} HT_SearchResult;

HT_SearchResult HashTableFind(HashTable * ht, const ht_val_t * const key)
{
	const size_t n_buckets = SIZE(ht->buckets);
	const ht_hash_t hash = ht->hash_function(key, n_buckets);

	HT_SearchResult sr;

	sr.bucket = ht->buckets.begin + hash;

	for(HT_Pair ** it = sr.bucket->begin; it != sr.bucket->end; ++it)
	{
		if((*it)->key == *key)
		{
			sr.pair = *it;
			return sr;
		}
	}

	sr.pair = NULL;
	return sr;
}

// "Private" function
void HT_Private_Repair(HashTable * ht, const HT_Pair * const old_begin)
{
	for(HT_Bucket * b = ht->buckets.begin; b != ht->buckets.end; ++b)
	{
		for(HT_Pair ** it = b->begin; it != b->end; ++it)
		{
			const ptrdiff_t index = (*it) - old_begin;
			(*it) = ht->data.begin + index;
		}
	}
}

// "Private" function
HT_Pair * HT_Private_Expand(HashTable * ht)
{
	const size_t size = SIZE(ht->data);
	if(size*1.3 >= ht->data.capacity)
	{
		const HT_Pair * old_begin = ht->data.begin;

		const size_t new_size = 2*size;
		RESERVE(ht->data, new_size); // This may invalidate references. They need be repaired.

		if(ht->data.begin != old_begin)
		{
			HT_Private_Repair(ht, old_begin);
		}
	}

	++ht->data.end;
	return ht->data.end - 1;
}

ht_val_t * pFindOrEmplace(HashTable * ht, const ht_val_t * const key, const ht_val_t * const default_value)
{
	HT_SearchResult loc = HashTableFind(ht, key);
	if(loc.pair) {
		return &loc.pair->value;
	}

	HT_Pair * back = HT_Private_Expand(ht);
	back->key = *key;
	back->value = *default_value;
	
	PUSH(*loc.bucket, back);

	return &back->value;
}

ht_val_t * FindOrEmplace(HashTable * ht, const ht_val_t key, const ht_val_t default_value)
{
	HT_SearchResult loc = HashTableFind(ht, &key);
	if(loc.pair) {
		return &loc.pair->value;
	}

	HT_Pair * back = HT_Private_Expand(ht);
	back->key = key;
	back->value = default_value;
	
	PUSH(*loc.bucket, back);

	return &back->value;
}

ht_val_t * HashTableRemove(HashTable * ht, const ht_val_t key)
{
	HT_SearchResult loc = HashTableFind(ht, &key);
	if(!loc.pair) return NULL;

	// Removing from data
	SWAP(HT_Pair, loc.pair, loc.bucket->end[-1]);
	--ht->data.end;
	
	// Updating bucket references
	const ptrdiff_t pos = loc.pair - *loc.bucket->begin;
	HT_Pair ** it = loc.bucket->begin + pos;
	SWAP(HT_Pair*, it, loc.bucket->end-1);
	--loc.bucket->end;

	return &loc.pair->value;
}


#endif
