#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/hash_table.h"

TEMPLATE_HASH_TABLE       (size_t, int, int, HashTable);

HT_DEFINE_DEFAULT_COMPARE (size_t, int, int, HashTable)
HT_DEFINE_NEW_AND_CLEAR   (size_t, int, int, HashTable)
HT_DEFINE_PRINT           (size_t, int, int, HashTable)
HT_DEFINE_HASH_INTEGERS   (size_t, int, int, HashTable)
HT_DEFINE_FIND            (size_t, int, int, HashTable)
HT_DEFINE_FIND_OR_EMPLACE (size_t, int, int, HashTable)
HT_DEFINE_REMOVE          (size_t, int, int, HashTable)


typedef char* cstring; // Pointer types need to be typedef'd
TEMPLATE_HASH_TABLE          (size_t, cstring, long int, Map);

int CompareStringPtrs(const cstring * const A, const cstring * const B) {
    return strcmp(*A, *B);
}

HT_DEFINE_SET_COMPARISON  (size_t, cstring, long int, Map)
HT_DEFINE_NEW_AND_CLEAR   (size_t, cstring, long int, Map)
HT_DEFINE_PRINT           (size_t, cstring, long int, Map)
HT_DEFINE_HASH_STRINGS    (size_t, cstring, long int, Map)
HT_DEFINE_FIND            (size_t, cstring, long int, Map)
HT_DEFINE_FIND_OR_EMPLACE (size_t, cstring, long int, Map)
HT_DEFINE_REMOVE          (size_t, cstring, long int, Map)


typedef struct {int real; int imag;} Complex; // Can use custom classes

int CompareComplex(const Complex * const A, const Complex * const B)
{
    int is_same = A->real == B->real && A->imag == B->imag;
    return !is_same;
}

size_t HashComplex(const Complex * const key, const size_t n_buckets)
{
    const long int mod = key->real*key->real + key->imag*key->imag;
    return mod % n_buckets;
}

TEMPLATE_HASH_TABLE       (size_t, Complex, int, ComplexMapping);

HT_DEFINE_SET_COMPARISON  (size_t, Complex, int, ComplexMapping)
HT_DEFINE_NEW_AND_CLEAR   (size_t, Complex, int, ComplexMapping)
HT_DEFINE_FIND            (size_t, Complex, int, ComplexMapping)
HT_DEFINE_FIND_OR_EMPLACE (size_t, Complex, int, ComplexMapping)
HT_DEFINE_REMOVE          (size_t, Complex, int, ComplexMapping)


int main()
{
    /**********************************************
       
       Playing with [string, int] map

    **********************************************/

    HashTable ht = NewHashTable(HashTableHashIntegers);
    
    *HashTableFindOrEmplace(&ht, 3, 0) += 5;

    HashTableFindOrEmplace(&ht, 7, 70);
    HashTableFindOrEmplace(&ht, 64, 640);
    HashTableFindOrEmplace(&ht, 1500, 15000);
    HashTableFindOrEmplace(&ht, 37, 370);
    HashTableFindOrEmplace(&ht, -5, -50);
    HashTableFindOrEmplace(&ht, -16, -160);
    HashTableFindOrEmplace(&ht, -64096821, -640968210);
    HashTableFindOrEmplace(&ht, -333, -3330);

    *HashTableFindOrEmplace(&ht, 3, 0) += 25;

    HashTableRemove(&ht, 3);

    HashTablePrint(&ht, "%d", "%d");

    ClearHashTable(&ht);

    /**********************************************
       
       Playing with [string, int] map

    **********************************************/

    Map map = NewMap(MapHashStrings);
    MapSetComparison(&map, CompareStringPtrs);
    
    *MapFindOrEmplace(&map, "3", 0) += 5;

    MapFindOrEmplace(&map, "7", 70);
    MapFindOrEmplace(&map, "64", 640);
    MapFindOrEmplace(&map, "1500", 15000);
    MapFindOrEmplace(&map, "37", 370);
    MapFindOrEmplace(&map, "-5", -50);
    MapFindOrEmplace(&map, "-16", -160);
    MapFindOrEmplace(&map, "-64096821", -640968210);
    MapFindOrEmplace(&map, "-333", -3330);

    *MapFindOrEmplace(&map, "3", 0) += 25;

    MapRemove(&map, "3");

    MapPrint(&map, "%s", "%d");

    ClearMap(&map);

    /**********************************************
       
       Playing with [string, int] map

    **********************************************/

    ComplexMapping cmap = NewComplexMapping(HashComplex);
    ComplexMappingSetComparison(&cmap, CompareComplex);

    Complex a, b, c, d, e;

    a.real = 3;     a.imag = -3;
    b.real = 5;     b.imag = -7;
    c.real = 9;     c.imag = -15;
    d.real = 0;     d.imag =  5;

    e = a;

    *ComplexMappingFindOrAllocate(&cmap, a) = 100; 
    // ^ This function takes no default value. Can be useful to avoid expensive copies

    ComplexMappingFindOrEmplace(&cmap, b, 2);
    ComplexMappingFindOrEmplace(&cmap, c, 3);
    ComplexMappingFindOrEmplace(&cmap, d, 4);

    *ComplexMappingFindOrEmplace(&cmap, e, 5) += 100;
    
    ComplexMappingRemove(&cmap, b);

    // Cannot use print function because it depends on printf
    printf("ComplexMapping {\n");
    for(ComplexMappingPair *it=cmap.data.begin; it != cmap.data.end; ++it)
    {
        printf("    %d%+di\t-->\t%d\n",
               it->key.real,
               it->key.imag,
               it->value);
    }
    printf("}\n");

    ClearComplexMapping(&cmap);

    return EXIT_SUCCESS;
}
