#ifndef DAY_18_H
#define DAY_18_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "common/testing.h"

typedef int Int;		// Just in case we have to switch to long
typedef int solution_t; // Just in case we have to switch to long

struct snail_number {
    Int data_left;
    Int data_right;
    struct snail_number * parent;
    struct snail_number * left;
    struct snail_number * right;
};

typedef struct snail_number* SnailNumber; // SnailNumbers always allocated on the heap

SnailNumber NewSnailNumber();
void ClearSnailNumber(SnailNumber* it);

void SetLeftChild(SnailNumber parent, SnailNumber child);
void SetRightChild(SnailNumber parent, SnailNumber child);


SnailNumber ReadSnailNumber(char ** it);
void PrintSnailNumber(SnailNumber num);

SnailNumber Split(Int * datum);

Int * LeftMostLeaf(SnailNumber num);
Int * RightMostLeaf(SnailNumber num);

/** 
 * When traversing the tree from leaves to root, this funcion finds the first
 * num to be a left-child
 */
SnailNumber FirstLeftAncestor(SnailNumber num);
/** 
 * When traversing the towards the root, this funcion finds the first
 * num to be a right-child
 */
SnailNumber FirstRightAncestor(SnailNumber num);

Int * LeftPropagation(SnailNumber num);
Int * RightPropagation(SnailNumber num);

bool Explode(SnailNumber* it);

bool ReduceExplode(SnailNumber * it, size_t depth);
bool ReduceSplit(SnailNumber * it, size_t depth);

void LeftAccumulate(SnailNumber* accumulator, SnailNumber* it);
solution_t Magnitude(SnailNumber num);
SnailNumber CopySnailNumber(SnailNumber orig);

// Solving
solution_t SolvePart1(const bool is_test);
solution_t SolvePart2(const bool is_test);

DECLARE_TEST(1);
DECLARE_TEST(2);

#endif
