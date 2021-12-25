#include "day24.h"
#include "common/vector.h"

#include "simulate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MONAD_DIGITS 14

Node NewNode(size_t id)
{
    Node node;
    node.id = id;    
    return node;
}

Tree NewTree()
{
    Tree t;

    for(size_t i=0; i < MONAD_DIGITS+1; ++i)
    {
        NEW_VECTOR(t.levels[i]);
    }

    PUSH(t.levels[0], NewNode(0)); // Root node
    t.levels[0].begin[0].z = 0;

    return t;
}

void ClearTree(Tree * t)
{
    for(size_t i=0; i < MONAD_DIGITS+1; ++i)
    {
        CLEAR(t->levels[i]);
    }
}

int CompareNodes(Node const * A, Node const * B)
{
    return A->id != B->id;
}


Node * FindNode(NodeVector * tree_level, Int z)
{
    for(Node * it = tree_level->begin; it != tree_level->end; ++it)
    {
        if(it->z == z) return it;
    }
    return NULL;
}

size_t GenerateID(Tree const * tree, size_t lvl)
{
    /* Id encoding
     *      101110101
     *      ~~~~~^^^^ Last 4 bits are the level
     *      ^ Leading bits are the position in the array
     */
    size_t pos = SIZE(tree->levels[lvl]);
    return lvl + (pos << 4);
}

size_t GetPositionFromId(size_t id)
{
    return id >> 4;
}

size_t GetLvlFromId(size_t id)
{
    return id & 0xF;
}

void ExploreNode(Tree * tree, ChunkData const chunkdata[MONAD_DIGITS], Node * node)
{
    size_t lvl = GetLvlFromId(node->id);

    for(size_t next_digit = 1; next_digit < 10; ++next_digit)
    {
        Int next_z = chunkdata[lvl].eval(node->z, next_digit);

        Node * it = FindNode(&tree->levels[lvl+1], next_z);

        if(it) // Collision. YAY!
        {
            node->children_id[next_digit-1] = it->id;
        }
        else // New path :(
        {
            PUSH(tree->levels[lvl+1], NewNode(GenerateID(tree, lvl+1)));
            tree->levels[lvl+1].end[-1].z = next_z;
        }
    }
}


void PrintLevel(Tree * tree, size_t lvl)
{
    printf("Level %ld:", lvl);
    for(Node * it = tree->levels[lvl].begin; it != tree->levels[lvl].end; ++it)
    {
        printf(" %ld", it->z);
    }
    printf("\n");
}


void ExploreTree(Tree * tree, ChunkData const chunkdata[MONAD_DIGITS])
{
    for(size_t lvl=0; lvl < MONAD_DIGITS; ++lvl)
    {
        for(Node * it = tree->levels[lvl].begin; it != tree->levels[lvl].end; ++it)
        {
            ExploreNode(tree, chunkdata, it);
        }

        printf("Level %ld has %ld paths\n", lvl, SIZE(tree->levels[lvl]));
    }
    printf("Level %d has %ld paths\n", MONAD_DIGITS, SIZE(tree->levels[MONAD_DIGITS]));
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
