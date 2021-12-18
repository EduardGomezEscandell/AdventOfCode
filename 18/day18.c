#include "day18.h"
#include "common/file_utils.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "common/vector.h"

typedef int Int;

struct snail_number {
    Int data_left;
    Int data_right;
    struct snail_number * parent;
    struct snail_number * left;
    struct snail_number * right;
};

typedef struct snail_number* SnailNumber; // SnailNumbers always allocated on the heap

SnailNumber NewSnailNumber() {
    SnailNumber p = malloc(sizeof(*p));
    
    p->left = NULL;
    p->right = NULL;
    p->parent = NULL;

    p->data_left = 0;
    p->data_right = 0;

    return p;
}

void SetLeftChild(SnailNumber parent, SnailNumber child)
{
    parent->left = child;
    parent->data_left = 0;
    child->parent = parent;
}

void SetRightChild(SnailNumber parent, SnailNumber child)
{
    parent->right = child;
    parent->data_right = 0;
    child->parent = parent;
}

void ClearSnailNumber(SnailNumber* it)
{
    if(!it) return;

    SnailNumber num = *it;
    if(!num) return;

    ClearSnailNumber(&num->left);
    ClearSnailNumber(&num->right);
    free(num);

    *it = NULL;
}

void PrintIndent(size_t depth)
{
    for(size_t i=0; i<4*depth; ++i) printf(" ");
}

void PrintSnailNumber(SnailNumber num)
{
    printf("[");
    
    if(num->left) PrintSnailNumber(num->left);
    else           printf("%d", num->data_left);
    
    printf(",");

    if(num->right) PrintSnailNumber(num->right);
    else           printf("%d", num->data_right);

    printf("]");
}

SnailNumber ReadSnailNumber(char ** it)
{
    ++*it; // Skipping opening braces

    SnailNumber p = NewSnailNumber();

    if(**it == '[')
    {
        SetLeftChild(p, ReadSnailNumber(it));
    }
    else
    {
        p->data_left = **it - '0';
        ++*it;        
    }
    ++*it; // Skipping comma

    if(**it == '[')
    {
        SetRightChild(p, ReadSnailNumber(it));
    } 
    else
    {
        p->data_right = **it - '0';
        ++*it;
    }

    ++*it; // Skipping closing braces

    return p;
}

SnailNumber Split(Int * datum)
{
    SnailNumber result = NewSnailNumber();
    result->data_left = *datum / 2;
    result->data_right = (*datum+1) / 2;

    *datum = 0;
    return result;
}

Int * LeftMostLeaf(SnailNumber num)
{
    if(num->left) return LeftMostLeaf(num->left);

    return &num->data_left;
}

Int * RightMostLeaf(SnailNumber num)
{
    if(num->right) return RightMostLeaf(num->right);

    return &num->data_right;
}

/** 
 * When traversing the tree from leaves to root, this funcion finds the first
 * num to be a left-child
 */
SnailNumber FirstLeftAncestor(SnailNumber num)
{
    SnailNumber parent = num->parent;
    if(!parent) return num;

    if(num == parent->left)
        return num;

    return FirstLeftAncestor(parent);
}

/** 
 * When traversing the towards the root, this funcion finds the first
 * num to be a right-child
 */
SnailNumber FirstRightAncestor(SnailNumber num)
{
    SnailNumber parent = num->parent;
    if(!parent) return num;

    if(num == parent->right)
        return num;
    
    return FirstRightAncestor(parent);
}

Int * LeftPropagation(SnailNumber num)
{
    SnailNumber ancestor = FirstRightAncestor(num)->parent;
    if(!ancestor) return NULL;

    if(ancestor->left)
        return RightMostLeaf(ancestor->left);
    else
        return &ancestor->data_left;
}

Int * RightPropagation(SnailNumber num){
    SnailNumber ancestor = FirstLeftAncestor(num)->parent;
    if(!ancestor) return NULL;

    if(ancestor->right)
        return LeftMostLeaf(ancestor->right);
    else
        return &ancestor->data_right;
}

bool Explode(SnailNumber* it)
{
    SnailNumber num = *it;

    if(num->left || num->right) {
        fprintf(stderr, "Attempting to explode a non-leaf!");
        exit(EXIT_FAILURE);
    }

    // Propagating left
    Int * prop;
    if((prop = LeftPropagation(*it)) != NULL)
        *prop += num->data_left;
    
    // Propagating right
    if((prop = RightPropagation(*it)) != NULL)
        *prop += num->data_right;

    ClearSnailNumber(it);

    return true;
}

bool ReduceExplode(SnailNumber * it, size_t depth)
{
    SnailNumber num = *it;

    if(num->left  && ReduceExplode(&num->left, depth+1)) return true;
    if(num->right && ReduceExplode(&num->right, depth+1)) return true;

    if(depth >= 4) return Explode(it);

    return false;
}

bool ReduceSplit(SnailNumber * it, size_t depth)
{
    SnailNumber num = *it;

    if(num->left  && ReduceSplit(&num->left, depth+1)) return true;
    if(!num->left && num->data_left > 9) {
        SetLeftChild(num, Split(&num->data_left));
        return true;
    }

    if(num->right && ReduceSplit(&num->right, depth+1)) return true;

    if(!num->right && num->data_right > 9) {
        SetRightChild(num, Split(&num->data_right));
        return true;
    }


    return false;
}

void LeftAccumulate(SnailNumber* accumulator, SnailNumber it)
{
    if(!*accumulator)  // First accumulation
    {
        *accumulator = it;
        it->parent = NULL;
    }
    else 
    {
        SnailNumber new_num = NewSnailNumber();
        SetLeftChild(new_num, *accumulator);
        SetRightChild(new_num, it);

        *accumulator = new_num;
    }
    
    while(true)
    {
        if (ReduceExplode(accumulator, 0)) continue;
        if (ReduceSplit(accumulator, 0)) continue;

        break;
    }
}

solution_t Magnitude(SnailNumber num)
{
    solution_t mag = 0;

    mag += 3 * ((num->left)  ? Magnitude(num->left)  : num->data_left);
    mag += 2 * ((num->right) ? Magnitude(num->right) : num->data_right);

    return mag;
}

solution_t SolvePart1(const bool is_test)
{
    FILE * file = GetFile(is_test, 18);

    size_t len = 0;
    char * line = NULL;

    SnailNumber acc = NULL;
    
    while(getline(&line, &len, file) != -1)
    {
        char * it = line;
        if(len == 0) continue;
        SnailNumber p = ReadSnailNumber(&it);

        LeftAccumulate(&acc, p);
    }

    solution_t magnitude = Magnitude(acc);


    ClearSnailNumber(&acc);
    free(line);
    fclose(file);

    return magnitude;
}

TEMPLATE_VECTOR(struct snail_number) SnailNumberVector;

solution_t SolvePart2(const bool is_test)
{
    // FILE * file = GetFile(is_test, 18);

    // size_t len = 0;
    // char * line = NULL;

    // SnailNumberVector numbers;
    
    // while(getline(&line, &len, file) != -1)
    // {
    //     char * it = line;
    //     if(len == 0) continue;
    //     SnailNumber p = ReadSnailNumber(&it);

    //     LeftAccumulate(&acc, p);
    // }

    // solution_t magnitude = Magnitude(acc);


    // ClearSnailNumber(&acc);
    // free(line);
    // fclose(file);

    return is_test;
}



DEFINE_TEST(1, 4140)
DEFINE_TEST(2, 3993)