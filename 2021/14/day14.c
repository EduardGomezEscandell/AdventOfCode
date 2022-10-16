#include "day14.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "common/file_utils.h"
#include "common/math.h"
#include "common/forward_list.h"
#include "common/hash_table.h"
#include "common/vector.h"


DEFINE_TEST(1, 1588)
DEFINE_TEST(2, 2188189693529)

// Templates
DEFINE_QUICKSORT_COMP(InstructionQuickSort, Instruction, CompareInstructions)


HT_DEFINE_SET_COMPARISON    (size_t, Target, count_t, Polymer)
HT_DEFINE_NEW_AND_CLEAR     (size_t, Target, count_t, Polymer)
HT_DEFINE_FIND              (size_t, Target, count_t, Polymer)
HT_DEFINE_FIND_OR_EMPLACE   (size_t, Target, count_t, Polymer)
HT_DEFINE_RESERVE           (size_t, Target, count_t, Polymer)
HT_DEFINE_REMOVE            (size_t, Target, count_t, Polymer)

HT_DEFINE_DEFAULT_COMPARE   (size_t, char, count_t, FreqMap)
HT_DEFINE_NEW_AND_CLEAR     (size_t, char, count_t, FreqMap)
HT_DEFINE_HASH_INTEGERS     (size_t, char, count_t, FreqMap)
HT_DEFINE_FIND              (size_t, char, count_t, FreqMap)
HT_DEFINE_FIND_OR_EMPLACE   (size_t, char, count_t, FreqMap)
HT_DEFINE_RESERVE           (size_t, char, count_t, FreqMap)
HT_DEFINE_REMOVE            (size_t, char, count_t, FreqMap)

LIST_DEFINE_NEW_NODE(char, List)
LIST_DEFINE_NEW_LIST(char, List)
LIST_DEFINE_INSERT(char, List)
LIST_DEFINE_EMPLACE(char, List)
LIST_DEFINE_CLEAR(char, List)
// LIST_DEFINE_PRINT(char, List)

Instruction *FindInstruction(
    Instruction * begin,
    Instruction * end,
    const Target * search)
{ // Binary search. Instruction vector must be sorted.
    if (end - begin < 5) {
        Instruction *guess = begin + (end - begin) / 2;
        
        int comparisson = CompareTargets(&guess->target, search);
        
        if (comparisson == 1)
            return FindInstruction(begin, guess, search);
        if (comparisson == 0)
            return guess;
        
        return FindInstruction(begin, guess, search);
    }
    for (Instruction *it = begin; it != end; ++it)
    {
        if (CompareTargets(&it->target, search) == 0)
            return it;
    }
    return NULL;
}


List ListFromString(const char * const line) {
    List list = NewList();


    ListNode * prev = NULL;
    for(char const * it=line; *it != '\0' && *it != '\n'; ++it)
    {
        ListNode * new_node = NewListNode(it);

        if(prev == NULL)
            list.begin = new_node;
        else
            ListInstert(prev, new_node);

        prev = new_node;
    }

    return list;
}

List ReadPolymerTemplate(FILE * file)
{
    char * line = NULL;
    size_t len = 0;
    ssize_t read = 0;

    List list;
    
    if((read = getline(&line, &len, file)) != -1) {       
        list = ListFromString(line);
    } else {
        fprintf(stderr, "Failed to read polymer template");
        exit(EXIT_FAILURE);
    }

    if((read = getline(&line, &len, file)) == -1) {
        fprintf(stderr, "Failed to skip line after polymer template");
        exit(EXIT_FAILURE);
    }

    free(line);

    return list;
}

InstructionVector ReadInstructions(FILE * file)
{
    char * line = NULL;
    size_t len = 0;
    ssize_t read = 0;

    InstructionVector v;
    NEW_VECTOR(v);
    
    while((read = getline(&line, &len, file)) != -1)
    {
        Instruction inst;
        inst.target.left = line[0];
        inst.target.right = line[1];
        inst.insert = line[6];

        PUSH(v, inst);
    }

    free(line);

    return v;
}

int CompareInstructions(const Instruction * a, const Instruction * b)
{
    if(a->target.left > b->target.left) return 1;
    if(a->target.left == b->target.left)
    {
        if(a->target.right > b->target.right) return 1;
        if(a->target.right == b->target.right) return 0;
    }
    return -1;
}

int CompareTargets(const Target * a, const Target * b)
{
    if(a->left > b->left) return 1;
    if(a->left == b->left)
    {
        if(a->right > b->right) return 1;
        if(a->right == b->right) return 0;
    }
    return -1;
}

void NextStep(List * polymer, const InstructionVector * instructions)
{
    if(polymer->begin == NULL || polymer->begin->next == NULL)
    {
        fprintf(stderr, "Polymer must have at least two monomers!");
        exit(EXIT_FAILURE);
    }

    Target searched;
    Instruction const * result;

    ListNode * node1 = polymer->begin;
    ListNode * node2 = node1->next;

    while(node2)
    {
        searched.left  = node1->data;
        searched.right = node2->data;

        result = FindInstruction(instructions->begin, instructions->end, &searched);
        if(!result) continue;

        ListEmplace(node1, &result->insert);

        node1 = node2;
        node2 = node1->next;
    }
}

FreqMap CountFrequecies(List * polymer)
{
    FreqMap freqs = NewFreqMap(FreqMapHashIntegers);

    ListNode const * curr = polymer->begin;
    while(curr)
    {
        *FreqMapFindOrEmplace(&freqs, curr->data, 0) += 1;
        curr = curr->next;
    }

    return freqs;
}


solution_t SolvePart1(const bool is_test)
{
    FILE * file = GetFile(is_test, 14);

    List polymer = ReadPolymerTemplate(file);

    InstructionVector instructions = ReadInstructions(file);
    InstructionQuickSort(instructions.begin, instructions.end);

    const size_t n_steps = 10;
    for (size_t step=0; step < n_steps; ++step) {
        NextStep(&polymer, &instructions);
    }

    FreqMap freqs = CountFrequecies(&polymer);

    count_t max = 0;
    count_t min = LONG_MAX;

    for (const FreqMapPair * it = freqs.data.begin; it != freqs.data.end; ++it) {
        if(it->value == 0) continue;
        
        max = MAX(max, it->value);
        min = MIN(min, it->value);
    }

    ClearFreqMap(&freqs);
    CLEAR(instructions);
    ListClear(&polymer);

    fclose(file);
    return max - min;
}

size_t HashTarget(const Target * A, size_t n_buckets)
{
    return (A->left * A->left + A->right * A->right) % n_buckets;
}

Polymer ReadPolymerTemplateOptimized(FILE * file, FreqMap * frequencies)
{
    char * line = NULL;
    size_t len = 0;
    ssize_t read = 0;

    Polymer polymer = NewPolymer(HashTarget);
    PolymerSetComparison(&polymer, CompareTargets);

    *frequencies = NewFreqMap(FreqMapHashIntegers);
    
    if((read = getline(&line, &len, file)) != -1) {       
        
        *FreqMapFindOrEmplace(frequencies, *line, 0) += 1;
        
        for(char * it = line+1; *it != '\0' && *it!='\n'; ++it)
        {
            Target target = {
                .left = it[-1],
                .right = it[0]
            };

            *PolymerFindOrEmplace(&polymer, target, 0) += 1;
            *FreqMapFindOrEmplace(frequencies, *it, 0) += 1;
        }

    } else {
        fprintf(stderr, "Failed to read polymer template");
        exit(EXIT_FAILURE);
    }

    if((read = getline(&line, &len, file)) == -1) {
        fprintf(stderr, "Failed to skip line after polymer template");
        exit(EXIT_FAILURE);
    }

    free(line);

    return polymer;
}

void NextStepOptimized(Polymer * polymer, const InstructionVector * instructions, FreqMap * frequencies)
{
    Polymer new_inserts = NewPolymer(HashTarget);
    PolymerSetComparison(&new_inserts, polymer->Compare);

    PolymerReserve(&new_inserts, SIZE(polymer->data));

    for(PolymerPair * it = polymer->data.begin; it != polymer->data.end; ++it)
    {
        const Target * target = &it->key;
        const count_t count = it->value;
        
        Instruction const * result = FindInstruction(instructions->begin, instructions->end, target);
        if(result == NULL)
        {
            *PolymerFindOrEmplace(&new_inserts, *target, 0) += count;
            continue;
        }
        
        char insert = result->insert;

        Target left = {
            .left = target->left,
            .right = insert
        };

        Target right = {
            .left  = insert,
            .right = target->right
        };

        *PolymerFindOrEmplace(&new_inserts, left, 0) += count;
        *PolymerFindOrEmplace(&new_inserts, right, 0) += count;
        *FreqMapFindOrEmplace(frequencies, insert, 0) += count;        
    }

    ClearPolymer(polymer);
    *polymer = new_inserts;
}

void PrintPolymer(const Polymer * polymer, const char * keyformat, const char * valueformat)
{
    printf("Polymer {\n");
    for(PolymerPair * it = polymer->data.begin; it != polymer->data.end; ++it)
    {
        printf("    ");
        printf(keyformat, (char) it->key.left, (char) it->key.right);
        printf(" : ");
        printf(valueformat, it->value);
        printf("\n");
    }
    printf("}\n");
}

solution_t SolvePart2(const bool is_test)
{
    FILE * file = GetFile(is_test, 14);

    FreqMap frequencies;

    Polymer polymer = ReadPolymerTemplateOptimized(file, &frequencies);

    InstructionVector instructions = ReadInstructions(file);
    InstructionQuickSort(instructions.begin, instructions.end);

    const size_t n_steps = 40;
    for (size_t step=0; step < n_steps; ++step) {
        NextStepOptimized(&polymer, &instructions, &frequencies);
    }

    count_t max = 0;
    count_t min = LONG_MAX;

    for (const FreqMapPair * it = frequencies.data.begin; it != frequencies.data.end; ++it)
    {
        if(it->value == 0) continue;
        
        max = MAX(max, it->value);
        min = MIN(min, it->value);
    }

    ClearFreqMap(&frequencies);
    CLEAR(instructions);
    ClearPolymer(&polymer);

    fclose(file);
    return max - min;
}
