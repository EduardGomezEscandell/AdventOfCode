#include "day24.h"
#include "common/hash_table.h"
#include "common/vector.h"

#include "simulate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MONAD_DIGITS 14

HT_DEFINE_DEFAULT_COMPARE    (size_t, Int, Node, Set)
HT_DEFINE_NEW_AND_CLEAR      (size_t, Int, Node, Set)
HT_DEFINE_FIND               (size_t, Int, Node, Set)
HT_DEFINE_FIND_OR_EMPLACE    (size_t, Int, Node, Set)
HT_DEFINE_HASH_INTEGERS      (size_t, Int, Node, Set)
HT_DEFINE_RESERVE            (size_t, Int, Node, Set)

Node NewNode(Int z)
{
    Node node;
    node.z = z;
    return node;
}

Tree NewTree()
{
    Tree t;

    for(size_t i=0; i < MONAD_DIGITS+1; ++i)
    {
        t.levels[i] = NewSet(SetHashIntegers);
    }

    *SetFindOrAllocate(&t.levels[0], 0) = NewNode(0); // Root node

    return t;
}

void ClearTree(Tree * t)
{
    for(size_t i=0; i < MONAD_DIGITS+1; ++i)
    {
        ClearSet(&t->levels[i]);
    }
}


void ExploreNode(Tree * tree, ChunkData const chunkdata[MONAD_DIGITS], Node * node, size_t lvl)
{
    for(size_t next_digit = 1; next_digit < 10; ++next_digit)
    {
        Int next_z = chunkdata[lvl].eval(next_digit, node->z);
        node->children[next_digit-1] = next_z;

        SetSearchResult it = SetFind(&tree->levels[lvl+1], &next_z);

        if(it.pair) continue; // Collision. YAY!
        
        // New path :(
        *SetFindOrAllocate(&tree->levels[lvl+1], next_z) = NewNode(next_z);        
    }
}


void PrintLevel(Tree * tree, size_t lvl)
{
    printf("Level %ld:", lvl);
    for(SetPair * it = tree->levels[lvl].data.begin; it != tree->levels[lvl].data.end; ++it)
    {
        printf(" %ld", it->key);
    }
    printf("\n");
}


void ExploreTree(Tree * tree, ChunkData const chunkdata[MONAD_DIGITS])
{
    for(size_t lvl=0; lvl < MONAD_DIGITS; ++lvl)
    {
        SetReserve(&tree->levels[lvl+1], 9 * SIZE(tree->levels[lvl].data));

        for(SetPair * it = tree->levels[lvl].data.begin; it != tree->levels[lvl].data.end; ++it)
        {
            ExploreNode(tree, chunkdata, &it->value, lvl);
        }

        printf("Level %ld has %ld paths\n", lvl, SIZE(tree->levels[lvl].data));
        fflush(stdout);
    }
    printf("Level %d has %ld paths\n", MONAD_DIGITS, SIZE(tree->levels[MONAD_DIGITS].data));
}


solution_t SolvePart1(const bool is_test)
{
    if(is_test) return 1;

    ChunkData chunkdata[MONAD_DIGITS];
    FillChunkData(chunkdata);
    Tree tree = NewTree();

    ExploreTree(&tree, chunkdata);

    printf("\n");
    PrintLevel(&tree, 0);
    PrintLevel(&tree, 1);
    PrintLevel(&tree, 2);
    printf("...\n");
    PrintLevel(&tree, 12);
    PrintLevel(&tree, 13);
    PrintLevel(&tree, 14);
    printf("\n");

    ClearTree(&tree);
    return 0;
}


solution_t SolvePart2(const bool is_test)
{
    return is_test;
}

DEFINE_TEST(1, 1)
DEFINE_TEST(2, 1)
