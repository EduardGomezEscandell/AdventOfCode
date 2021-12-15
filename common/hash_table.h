#ifndef HASH_TABLE_INCLUDED_H
#define HASH_TABLE_INCLUDED_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "vector.h"

#define TEMPLATE_HASH_TABLE(HT_hash_t, HT_key_t, HT_val_t, HT_name)             \
                                                                                \
typedef struct {                                                                \
    HT_key_t key;                                                               \
    HT_val_t value;                                                             \
} HT_name##Pair;                                                                \
                                                                                \
TEMPLATE_VECTOR(HT_name##Pair*) HT_name##Bucket;                                \
TEMPLATE_VECTOR(HT_name##Bucket) HT_name##BucketVector;                         \
TEMPLATE_VECTOR(HT_name##Pair) HT_name##PairVector;                             \
                                                                                \
typedef struct HT_name##SearchResult{                                           \
    HT_name##Bucket * bucket;                                                   \
    HT_name##Pair * pair;                                                       \
} HT_name##SearchResult;                                                        \
                                                                                \
typedef HT_hash_t                                                               \
    (*HT_name##HashFun)(const HT_key_t * const key, const size_t n_buckets);    \
typedef int  (*HT_name##CompFun)(                                               \
    const HT_key_t * const keyA,                                                \
    const HT_key_t * const keyB);                                               \
                                                                                \
typedef struct HT_name{                                                         \
    HT_name##BucketVector buckets;                                              \
    HT_name##PairVector data;                                                   \
    HT_name##HashFun Hash;                                                      \
    HT_name##CompFun Compare;                                                   \
} HT_name;                                                                      \
                                                                                \
HT_name##Bucket HT_name##NewBucket();                                           \
HT_name New##HT_name(HT_name##HashFun hash_function);                           \
void Clear##HT_name(HT_name * ht);                                              \
void Print##HT_name(                                                            \
    HT_name * const ht,                                                         \
    const char * key_format,                                                    \
    const char * value_format);                                                 \
HT_name##SearchResult HT_name##Find(HT_name * ht, const HT_key_t * const key);  \
HT_val_t * HT_name##FindOrEmplacePtr(                                           \
    HT_name * ht,                                                               \
    const HT_key_t * const key,                                                 \
    const HT_val_t * const default_value);                                      \
HT_val_t * HT_name##FindOrEmplace(                                              \
    HT_name * ht,                                                               \
    const HT_key_t key,                                                         \
    const HT_val_t default_value);                                              \
HT_val_t * HT_name##Remove(HT_name * ht, const HT_key_t key);                   \
void HT_name##_Private_Repair(                                                  \
    HT_name * ht,                                                               \
    const HT_name##Pair * const old_begin);                                     \
HT_name##Pair * HT_name##_Private_Expand(HT_name * ht);                         \
void HT_name##SetComparison(HT_name * ht, HT_name##CompFun comp_fun);           \
int HT_name##DefaultCompare(                                                    \
    const HT_key_t * const keyA,                                                \
    const HT_key_t * const keyB);                                               \
HT_hash_t HT_name##HashIntegers(                                                \
    const HT_key_t * const key,                                                 \
    const size_t n_buckets);                                                    \
HT_hash_t HT_name##HashStrings(                                                 \
    const HT_key_t * const key,                                                 \
    const size_t n_buckets)



#define HT_DEFINE_NEW_AND_CLEAR_TABLE(HT_hash_t, HT_key_t, HT_val_t, HT_name)   \
HT_name##Bucket HT_name##NewBucket() {                                          \
    HT_name##Bucket bucket;                                                     \
    bucket.capacity = 1;                                                        \
    bucket.begin = malloc(bucket.capacity * sizeof(*bucket.begin));             \
    bucket.end = bucket.begin;                                                  \
    return bucket;                                                              \
}                                                                               \
                                                                                \
int HT_name##DefaultCompare(                                                    \
    const HT_key_t * const keyA,                                                \
    const HT_key_t * const keyB)                                                \
{                                                                               \
    return keyA == keyB ? 0 : 1;                                                \
}                                                                               \
                                                                                \
HT_name New##HT_name(HT_name##HashFun hash_function)                            \
{                                                                               \
    HT_name ht;                                                                 \
    NEW_VECTOR(ht.buckets);                                                     \
    NEW_VECTOR(ht.data);                                                        \
                                                                                \
    const size_t n_buckets = 8;                                                 \
    RESERVE(ht.buckets, n_buckets);                                             \
    ht.buckets.end += n_buckets;                                                \
                                                                                \
    for(HT_name##Bucket * it = ht.buckets.begin; it != ht.buckets.end; ++it)    \
        *it = HT_name##NewBucket();                                             \
                                                                                \
    ht.Hash = hash_function;                                                    \
    ht.Compare = HT_name##DefaultCompare;                                       \
                                                                                \
    return ht;                                                                  \
}                                                                               \
                                                                                \
void Clear##HT_name(HT_name * ht)                                               \
{                                                                               \
    for(HT_name##Bucket * it = ht->buckets.begin; it != ht->buckets.end; ++it)  \
        CLEAR(*it);                                                             \
    CLEAR(ht->buckets);                                                         \
    CLEAR(ht->data);                                                            \
    ht->Hash = NULL;                                                            \
    ht->Compare = NULL;                                                         \
}

#define HT_DEFINE_PRINT(HT_hash_t, HT_key_t, HT_val_t, HT_name)                 \
void HT_name##Print(                                                            \
    HT_name * const ht,                                                         \
    const char * key_format,                                                    \
    const char * value_format)                                                  \
{                                                                               \
    printf("{\n");                                                              \
    for(HT_name##Bucket const* b =ht->buckets.begin; b != ht->buckets.end; ++b) \
    {                                                                           \
        for(HT_name##Pair * const * it = b->begin; it != b->end; ++it)          \
        {                                                                       \
            printf("    ");                                                     \
            printf(key_format, (*it)->key);                                     \
            printf(" : ");                                                      \
            printf(value_format, (*it)->value);                                 \
            printf("\n");                                                       \
        }                                                                       \
    }                                                                           \
    printf("}\n");                                                              \
}

#define HT_DEFINE_SET_COMPARISON(HT_hash_t, HT_key_t, HT_val_t, HT_name)        \
void HT_name##SetComparison(HT_name * ht, HT_name##CompFun comp_fun) {          \
    ht->Compare = comp_fun;                                                     \
}

#define HT_DEFINE_HASH_INTEGERS(HT_hash_t, HT_key_t, HT_val_t, HT_name)         \
HT_hash_t HT_name##HashIntegers(                                                \
    const HT_key_t * const key, const size_t n_buckets)                         \
{                                                                               \
    const HT_hash_t hash = (*key) * (*key);                                     \
    return hash % n_buckets;                                                    \
}

#define HT_DEFINE_HASH_STRINGS(HT_hash_t, HT_key_t, HT_val_t, HT_name)          \
HT_hash_t HT_name##HashStrings(                                                 \
    const HT_key_t * const key, const size_t n_buckets)                         \
{                                                                               \
    HT_hash_t hash = 0;                                                         \
    for(size_t i=0; (*key)[i]!='\0'; ++i)                                       \
        hash += (*key)[i]*(*key)[i];                                            \
    return hash % n_buckets;                                                    \
}

#define HT_DEFINE_FIND(HT_hash_t, HT_key_t, HT_val_t, HT_name)                  \
HT_name##SearchResult HT_name##Find(HT_name * ht, const HT_key_t * const key)   \
{                                                                               \
    const size_t n_buckets = SIZE(ht->buckets);                                 \
    const HT_hash_t hash = ht->Hash(key, n_buckets);                            \
                                                                                \
    HT_name##SearchResult sr;                                                   \
    sr.bucket = ht->buckets.begin + hash;                                       \
                                                                                \
    for(HT_name##Pair ** it = sr.bucket->begin; it != sr.bucket->end; ++it)     \
    {                                                                           \
        if(ht->Compare(&(*it)->key, key) ==0)                                   \
        {                                                                       \
            sr.pair = *it;                                                      \
            return sr;                                                          \
        }                                                                       \
    }                                                                           \
                                                                                \
    sr.pair = NULL;                                                             \
    return sr;                                                                  \
}

#define HT_DEFINE_FIND_OR_EMPLACE(HT_hash_t, HT_key_t, HT_val_t, HT_name)       \
void HT_name##_Private_Repair(HT_name * ht, const HT_name##Pair * const old_begin)   \
{                                                                               \
    for(HT_name##Bucket * b = ht->buckets.begin; b != ht->buckets.end; ++b)     \
    {                                                                           \
        for(HT_name##Pair ** it = b->begin; it != b->end; ++it)                 \
        {                                                                       \
            const ptrdiff_t index = (*it) - old_begin;                          \
            (*it) = ht->data.begin + index;                                     \
        }                                                                       \
    }                                                                           \
}                                                                               \
                                                                                \
HT_name##Pair * HT_name##_Private_Expand(HT_name * ht)                               \
{                                                                               \
    const size_t size = SIZE(ht->data);                                         \
    if(size*1.3 >= ht->data.capacity)                                           \
    {                                                                           \
        const HT_name##Pair * old_begin = ht->data.begin;                       \
                                                                                \
        const size_t new_size = 2*size;                                         \
        RESERVE(ht->data, new_size);                                            \
        /* ^This may invalidate references. They need be repaired.*/            \
        if(ht->data.begin != old_begin)                                         \
        {                                                                       \
            HT_name##_Private_Repair(ht, old_begin);                                   \
        }                                                                       \
    }                                                                           \
                                                                                \
    ++ht->data.end;                                                             \
    return ht->data.end - 1;                                                    \
}                                                                               \
                                                                                \
HT_val_t * HT_name##FindOrEmplacePtr(                                           \
    HT_name * ht,                                                             \
    const HT_key_t * const key,                                                 \
    const HT_val_t * const default_value)                                       \
{                                                                               \
    HT_name##SearchResult loc = HT_name##Find(ht, key);                         \
    if(loc.pair) {                                                              \
        return &loc.pair->value;                                                \
    }                                                                           \
                                                                                \
    HT_name##Pair * back = HT_name##_Private_Expand(ht);                               \
    back->key = *key;                                                           \
    back->value = *default_value;                                               \
                                                                                \
    PUSH(*loc.bucket, back);                                                    \
                                                                                \
    return &back->value;                                                        \
}                                                                               \
                                                                                \
HT_val_t * HT_name##FindOrEmplace(                                              \
    HT_name * ht,                                                             \
    const HT_key_t key,                                                         \
    const HT_val_t default_value)                                               \
{                                                                               \
    HT_name##SearchResult loc = HT_name##Find(ht, &key);                        \
    if(loc.pair) {                                                              \
        return &loc.pair->value;                                                \
    }                                                                           \
                                                                                \
    HT_name##Pair * back = HT_name##_Private_Expand(ht);                               \
    back->key = key;                                                            \
    back->value = default_value;                                                \
                                                                                \
    PUSH(*loc.bucket, back);                                                    \
                                                                                \
    return &back->value;                                                        \
}

#define HT_DEFINE_REMOVE(HT_hash_t, HT_key_t, HT_val_t, HT_name)                \
HT_val_t * HT_name##Remove(HT_name * ht, const HT_key_t key)                    \
{                                                                               \
    HT_name##SearchResult loc = HT_name##Find(ht, &key);                        \
    if(!loc.pair) return NULL;                                                  \
                                                                                \
    /* Removing from data */                                                    \
    SWAP(HT_name##Pair, loc.pair, loc.bucket->end[-1]);                         \
    --ht->data.end;                                                             \
                                                                                \
    /* Updating bucket references */                                            \
    const ptrdiff_t pos = loc.pair - *loc.bucket->begin;                        \
    HT_name##Pair ** it = loc.bucket->begin + pos;                              \
    SWAP(HT_name##Pair*, it, loc.bucket->end-1);                                \
    --loc.bucket->end;                                                          \
                                                                                \
    return &loc.pair->value;                                                    \
}                                                                               \


#endif
