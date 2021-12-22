#ifndef HASH_TABLE_INCLUDED_H
#define HASH_TABLE_INCLUDED_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "vector.h"
#include "math.h"

/******************************************************************************
 *           This template implements a hash-table based map                  *
 *                                                                            *
 * To use this template you must use in your header:                          *
 * TEMPLATE_HASH_TABLE(hash type, key type, value type, class name)           *
 *                                                                            *
 * Then, on a source file you must use:                                       *
 * HT_DEFINE_DEFAULT_COMPARE -or- HT_DEFINE_SET_COMPARISON                    *
 * ^to simply do A==B               ^For a custom comparator between keys     *
 *                                                                            *
 * Afterwards, you will also need:                                            *
 * HT_DEFINE_NEW_AND_CLEAR                                                    *
 * in order to have "constructor" and "destructor" for memory management.     *
 *                                                                            *
 * Other macros are used to define more functions.                            *
 * Avoid using functions with the word _Private_ in their name                *
 *                                                                            *
 * Any linker errors are likely to be due to missing a HT_DEFINE_* macro      *
 * For an example, see See /sample/map_sample.c                               *
 *****************************************************************************/


#define TEMPLATE_HASH_TABLE(HT_hash_t, HT_key_t, HT_val_t, HT_name)           \
                                                                              \
typedef struct {                                                              \
    HT_key_t key;                                                             \
    HT_val_t value;                                                           \
} HT_name##Pair;                                                              \
                                                                              \
TEMPLATE_VECTOR(HT_name##Pair*) HT_name##Bucket;                              \
TEMPLATE_VECTOR(HT_name##Bucket) HT_name##BucketVector;                       \
TEMPLATE_VECTOR(HT_name##Pair) HT_name##PairVector;                           \
                                                                              \
typedef struct HT_name##SearchResult{                                         \
    HT_name##Bucket * bucket;                                                 \
    HT_name##Pair * pair;                                                     \
} HT_name##SearchResult;                                                      \
                                                                              \
typedef HT_hash_t                                                             \
    (*HT_name##HashFun)(const HT_key_t * key, size_t n_buckets);              \
typedef int  (*HT_name##CompFun)(                                             \
    const HT_key_t * keyA,                                                    \
    const HT_key_t * keyB);                                                   \
                                                                              \
typedef struct HT_name{                                                       \
    HT_name##BucketVector buckets;                                            \
    HT_name##PairVector data;                                                 \
    HT_name##HashFun Hash;                                                    \
    HT_name##CompFun Compare;                                                 \
} HT_name;                                                                    \
                                                                              \
HT_name##Bucket HT_name##NewBucket();                                         \
HT_name New##HT_name(HT_name##HashFun hash_function);                         \
void Clear##HT_name(HT_name * ht);                                            \
void Print##HT_name(                                                          \
    const HT_name * ht,                                                       \
    const char * key_format,                                                  \
    const char * value_format);                                               \
HT_name##SearchResult HT_name##Find(HT_name * ht, const HT_key_t * key);      \
HT_val_t * HT_name##FindOrAllocate(                                           \
    HT_name * ht,                                                             \
    HT_key_t key);                                                            \
HT_val_t * HT_name##FindOrEmplace(                                            \
    HT_name * ht,                                                             \
    const HT_key_t key,                                                       \
    const HT_val_t default_value);                                            \
HT_val_t * HT_name##Remove(HT_name * ht, const HT_key_t key);                 \
void HT_name##_Private_Rehash(HT_name * ht, size_t n_buckets);                \
HT_name##Pair * HT_name##_Private_Expand(HT_name * ht);                       \
void HT_name##SetComparison(HT_name * ht, HT_name##CompFun comp_fun);         \
int HT_name##DefaultCompare(                                                  \
    const HT_key_t * keyA,                                                    \
    const HT_key_t * keyB);                                                   \
HT_hash_t HT_name##HashIntegers(                                              \
    const HT_key_t * key,                                                     \
    size_t n_buckets);                                                        \
HT_hash_t HT_name##HashStrings(                                               \
    const HT_key_t * key,                                                     \
    size_t n_buckets)


#define HT_DEFINE_DEFAULT_COMPARE(HT_hash_t, HT_key_t, HT_val_t, HT_name)     \
int HT_name##DefaultCompare(                                                  \
    const HT_key_t * keyA,                                                    \
    const HT_key_t * keyB)                                                    \
{                                                                             \
    return *keyA == *keyB ? 0 : 1;                                            \
}

#define HT_DEFINE_SET_COMPARISON(HT_hash_t, HT_key_t, HT_val_t, HT_name)      \
int HT_name##DefaultCompare(                                                  \
    const HT_key_t * keyA,                                                    \
    const HT_key_t * keyB)                                                    \
{                                                                             \
    fprintf(stderr,                                                           \
        "No comparison set for class "#HT_name" (%s:%d)\n",                   \
        __FILE__, __LINE__);                                                  \
    return keyA==keyB;                                                        \
}                                                                             \
void HT_name##SetComparison(HT_name * ht, HT_name##CompFun comp_fun) {        \
    ht->Compare = comp_fun;                                                   \
}

#define HT_DEFINE_RESERVE(HT_hash_t, HT_key_t, HT_val_t, HT_name)             \
void HT_name##Reserve(HT_name * ht, size_t new_capacity)                      \
{                                                                             \
    /* Input validation */                                                    \
    if(new_capacity < ht->data.capacity) return;                              \
                                                                              \
    const size_t curr_buckets = SIZE(ht->buckets);                            \
    size_t n_buckets = new_capacity * 0.6;                                    \
    n_buckets = MAX(n_buckets, curr_buckets);                                 \
                                                                              \
    /* Updating container */                                                  \
    RESERVE(ht->data, new_capacity);                                          \
    HT_name##_Private_Rehash(ht, n_buckets);                                  \
}

#define HT_DEFINE_NEW_AND_CLEAR(HT_hash_t, HT_key_t, HT_val_t, HT_name)       \
HT_name##Bucket HT_name##NewBucket() {                                        \
    HT_name##Bucket bucket;                                                   \
    bucket.capacity = 1;                                                      \
    bucket.begin = malloc(bucket.capacity * sizeof(*bucket.begin));           \
    bucket.end = bucket.begin;                                                \
    return bucket;                                                            \
}                                                                             \
                                                                              \
                                                                              \
HT_name New##HT_name(HT_name##HashFun hash_function)                          \
{                                                                             \
    HT_name ht;                                                               \
    NEW_VECTOR(ht.buckets);                                                   \
    NEW_VECTOR(ht.data);                                                      \
                                                                              \
    const size_t n_buckets = 8;                                               \
    RESERVE(ht.buckets, n_buckets);                                           \
    ht.buckets.end += n_buckets;                                              \
                                                                              \
    for(HT_name##Bucket * it = ht.buckets.begin; it != ht.buckets.end; ++it)  \
        *it = HT_name##NewBucket();                                           \
                                                                              \
    ht.Hash = hash_function;                                                  \
    ht.Compare = HT_name##DefaultCompare;                                     \
                                                                              \
    return ht;                                                                \
}                                                                             \
                                                                              \
void Clear##HT_name(HT_name * ht)                                             \
{                                                                             \
    for(HT_name##Bucket * it = ht->buckets.begin; it != ht->buckets.end; ++it)\
        CLEAR(*it);                                                           \
    CLEAR(ht->buckets);                                                       \
    CLEAR(ht->data);                                                          \
    ht->Hash = NULL;                                                          \
    ht->Compare = NULL;                                                       \
}                                                                             \
void HT_name##_Private_Rehash(HT_name * ht, size_t n_buckets)                 \
{                                                                             \
    for(HT_name##Bucket * it = ht->buckets.begin; it != ht->buckets.end; ++it)\
        CLEAR(*it);                                                           \
    CLEAR(ht->buckets);                                                       \
                                                                              \
    NEW_VECTOR(ht->buckets);                                                  \
    RESERVE(ht->buckets, n_buckets);                                          \
    ht->buckets.end += n_buckets;                                             \
                                                                              \
    for(HT_name##Bucket * it = ht->buckets.begin;                             \
        it != ht->buckets.end;                                                \
        ++it)                                                                 \
        *it = HT_name##NewBucket();                                           \
                                                                              \
    for(HT_name##Pair * it = ht->data.begin; it != ht->data.end; ++it)        \
    {                                                                         \
        const HT_hash_t hash = ht->Hash(&it->key, n_buckets);                 \
        PUSH(ht->buckets.begin[hash], it);                                    \
    }                                                                         \
}                                                                             \
                                                                              \

#define HT_DEFINE_PRINT(HT_hash_t, HT_key_t, HT_val_t, HT_name)               \
void HT_name##Print(                                                          \
    const HT_name * ht,                                                       \
    const char * key_format,                                                  \
    const char * value_format)                                                \
{                                                                             \
    printf(#HT_name" {\n");                                                   \
    for(const HT_name##Pair * it = ht->data.begin; it != ht->data.end; ++it)  \
    {                                                                         \
        printf("    ");                                                       \
        printf(key_format, it->key);                                          \
        printf(" : ");                                                        \
        printf(value_format, it->value);                                      \
        printf("\n");                                                         \
    }                                                                         \
    printf("}\n");                                                            \
}                                                                             \
                                                                              \
void HT_name##PrintDebug(                                                     \
    const HT_name * ht,                                                       \
    const char * key_format,                                                  \
    const char * value_format)                                                \
{                                                                             \
printf(#HT_name" internal structure: {\n");                                   \
    for(HT_name##Bucket * b = ht->buckets.begin; b!=ht->buckets.end; ++b)     \
    {                                                                         \
        printf("  Bucket with hash %ld {\n", b - ht->buckets.begin);          \
        for(HT_name##Pair ** it =  b->begin; it != b->end; ++it)              \
        {                                                                     \
            printf("    ");                                                   \
            printf(key_format, (*it)->key);                                   \
            printf(" : ");                                                    \
            printf(value_format, (*it)->value);                               \
            printf("\n");                                                     \
        }                                                                     \
        printf("  }\n");                                                      \
    }                                                                         \
    printf("}\n");                                                            \
}


#define HT_DEFINE_HASH_INTEGERS(HT_hash_t, HT_key_t, HT_val_t, HT_name)       \
HT_hash_t HT_name##HashIntegers(                                              \
    const HT_key_t * key, size_t n_buckets)                                   \
{                                                                             \
    const HT_hash_t hash = (*key) * (*key);                                   \
    return hash % n_buckets;                                                  \
}

#define HT_DEFINE_HASH_STRINGS(HT_hash_t, HT_key_t, HT_val_t, HT_name)        \
HT_hash_t HT_name##HashStrings(                                               \
    const HT_key_t * key, size_t n_buckets)                                   \
{                                                                             \
    HT_hash_t hash = 0;                                                       \
    for(size_t i=0; (*key)[i]!='\0'; ++i)                                     \
        hash += (*key)[i]*(*key)[i];                                          \
    return hash % n_buckets;                                                  \
}

#define HT_DEFINE_FIND(HT_hash_t, HT_key_t, HT_val_t, HT_name)                \
HT_name##SearchResult HT_name##Find(HT_name * ht, const HT_key_t * key)       \
{                                                                             \
    const size_t n_buckets = SIZE(ht->buckets);                               \
    const HT_hash_t hash = ht->Hash(key, n_buckets);                          \
                                                                              \
    HT_name##SearchResult sr;                                                 \
    sr.bucket = ht->buckets.begin + hash;                                     \
                                                                              \
    for(HT_name##Pair ** it = sr.bucket->begin; it != sr.bucket->end; ++it)   \
    {                                                                         \
        if(ht->Compare(&(*it)->key, key) ==0)                                 \
        {                                                                     \
            sr.pair = *it;                                                    \
            return sr;                                                        \
        }                                                                     \
    }                                                                         \
                                                                              \
    sr.pair = NULL;                                                           \
    return sr;                                                                \
}

#define HT_DEFINE_FIND_OR_EMPLACE(HT_hash_t, HT_key_t, HT_val_t, HT_name)     \
HT_name##Pair * HT_name##_Private_Expand(HT_name * ht)                        \
{                                                                             \
    const size_t size = SIZE(ht->data);                                       \
    if(size >= 0.7 * ht->data.capacity)                                       \
    {                                                                         \
        const size_t new_size = 2*size;                                       \
        RESERVE(ht->data, new_size);                                          \
        /* ^This may invalidate references. They need be repaired.*/          \
        HT_name##_Private_Rehash(ht, 1+2*SIZE(ht->buckets));                  \
        /* We use this to expand the number of buckets */                     \
    }                                                                         \
                                                                              \
    ++ht->data.end;                                                           \
    return ht->data.end - 1;                                                  \
}                                                                             \
                                                                              \
HT_val_t * HT_name##FindOrAllocate(                                           \
    HT_name * ht,                                                             \
    HT_key_t key)                                                             \
{                                                                             \
    HT_name##SearchResult loc = HT_name##Find(ht, &key);                      \
    if(loc.pair) {                                                            \
        return &loc.pair->value;                                              \
    }                                                                         \
                                                                              \
    const size_t old_size = SIZE(ht->buckets);                                \
    HT_name##Pair * back = HT_name##_Private_Expand(ht);                      \
                                                                              \
    if(old_size != SIZE(ht->buckets)) { /* Rehashing has happended */         \
        loc = HT_name##Find(ht, &key);                                        \
        if(loc.pair) {                                                        \
            return &loc.pair->value;                                          \
        }                                                                     \
    }                                                                         \
    back->key = key;                                                          \
                                                                              \
    PUSH(*loc.bucket, back);                                                  \
                                                                              \
    return &back->value;                                                      \
}                                                                             \
                                                                              \
HT_val_t * HT_name##FindOrEmplace(                                            \
    HT_name * ht,                                                             \
    const HT_key_t key,                                                       \
    const HT_val_t default_value)                                             \
{                                                                             \
    HT_name##Pair * prev_end = ht->data.end;                                  \
    HT_val_t * value = HT_name##FindOrAllocate(ht, key);                      \
    if(ht->data.end != prev_end) /* Allocated */                              \
        *value = default_value;                                               \
    return value;                                                             \
}


#define HT_DEFINE_REMOVE(HT_hash_t, HT_key_t, HT_val_t, HT_name)              \
HT_val_t * HT_name##Remove(HT_name * ht, const HT_key_t key)                  \
{                                                                             \
    HT_name##SearchResult loc = HT_name##Find(ht, &key);                      \
    if(!loc.pair) return NULL;                                                \
                                                                              \
    /* Updating bucket references */                                          \
    HT_name##Pair ** it = loc.bucket->begin;                                  \
    for(; it != loc.bucket->end; ++it)                                        \
        if(*it == loc.pair) break;                                            \
    *it = loc.bucket->end[-1];                                                \
    --loc.bucket->end;                                                        \
                                                                              \
    /* Removing from data */                                                  \
    *loc.pair = ht->data.end[-1];                                             \
    --ht->data.end;                                                           \
                                                                              \
    return &loc.pair->value;                                                  \
}                                                                             \


#endif
