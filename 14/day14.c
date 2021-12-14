#include "day14.h"

#include <stdio.h>
#include <stdlib.h>

#include "common/file_utils.h"
#include "common/forward_list.h"
#include "common/matrix.h"
#include "common/vector.h"


DEFINE_TEST(1, 1588)
DEFINE_TEST(2, 2188189693529)

DEFINE_FIND_COMP(Instruction, FindInstruction, CompareInstructions)

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
        inst.input[0] = line[0];
        inst.input[1] = line[1];
        inst.data     = line[6];

        PUSH(v, inst);
    }

    free(line);

    return v;
}

int CompareInstructions(const Instruction * const a, const Instruction * const b)
{
    if(a->input[0] > b->input[0]) return 1;
    if(a->input[0] == b->input[0])
    {
        if(a->input[1] > b->input[1]) return 1;
        if(a->input[1] == b->input[1]) return 0;
    }
    return -1;
}

DEFINE_QUICKSORT_COMP(InstructionQuickSort, Instruction, CompareInstructions)

void NextStep(List * polymer, const InstructionVector * instructions)
{
    if(polymer->begin == NULL || polymer->begin->next == NULL)
    {
        fprintf(stderr, "Polymer must have at least two monomers!");
        exit(EXIT_FAILURE);
    }

    Instruction searched;
    Instruction const * result;

    Node * node1 = polymer->begin;
    Node * node2 = node1->next;

    while(node2)
    {
        searched.input[0] = node1->data;
        searched.input[1] = node2->data;

        result = FindInstruction(instructions->begin, instructions->end, &searched, true);
        if(!result) continue;

        ListEmplace(node1, result->data);

        node1 = node2;
        node2 = node1->next;
    }
}

Vector CountFrequecies(List * polymer)
{
    Vector freqs;
    NEW_VECTOR(freqs);
    const size_t alphabet_size = 'Z' - 'A';
    RESERVE(freqs, alphabet_size);

    freqs.end = freqs.begin + alphabet_size;
    for(long long int * it=freqs.begin; it != freqs.end; ++it) *it = 0;

    Node const * curr = polymer->begin;
    while(curr)
    {
        freqs.begin[curr->data - 'A'] += 1;
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

    Vector freqs = CountFrequecies(&polymer);

    long long int max = freqs.begin[0];
    long long int min = freqs.begin[0];

    for (long long int *it = freqs.begin; it != freqs.end; ++it) {
        if(*it > max) max = *it;
        if(*it!=0 && *it < min) min = *it;
        if(min == 0 && *it != 0) min = *it;
    }

    CLEAR(freqs);
    CLEAR(instructions);
    ListClear(&polymer);

    fclose(file);
    return max - min;
}

SparseMatrix ReadPolymerTemplateOptimized(FILE * file, Vector * frequencies)
{
    char * line = NULL;
    size_t len = 0;
    ssize_t read = 0;

    SparseMatrix targets = NewSparseMatrix();

    const size_t alphabet_size = 'Z' - 'A';
    RESERVE(*frequencies, alphabet_size);
    frequencies->end = frequencies->begin + alphabet_size;
    for(long long int * it=frequencies->begin; it != frequencies->end; ++it) *it = 0;
    
    if((read = getline(&line, &len, file)) != -1) {       
        
        ++frequencies->begin[*line - 'A'];
        for(char * it = line+1; *it != '\0' && *it!='\n'; ++it)
        {
            SpTriplet target;
            target.row = it[-1];
            target.col = it[0];
            target.data = 1;

            SpPush(&targets, &target);
            ++frequencies->begin[*it - 'A'];
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

    return targets;
}

void NextStepOptimized(SparseMatrix * polymer, const InstructionVector * instructions, Vector * frequencies)
{
    Instruction searched;

    SparseMatrix new_entries = NewSparseMatrix();

    for(SpTriplet * it = polymer->begin; it != polymer->end; ++it)
    {
        searched.input[0] = it->row;
        searched.input[1] = it->col;

        Instruction const * result = FindInstruction(instructions->begin, instructions->end, &searched, true);

        if(result == NULL) continue;

        SpTriplet left;
        left.row = it->row;
        left.col = result->data;
        left.data = it->data;

        SpTriplet right;
        right.row = result->data;
        right.col = it->col;
        right.data = it->data;

        frequencies->begin[result->data - 'A'] += it->data;
        it->data = 0;
        
        SpPush(&new_entries, &left);
        SpPush(&new_entries, &right);
    }

    SpAppend(polymer, &new_entries);
    ClearSparseMatrix(&new_entries);

    SpMergeDuplicates(polymer);
    SpPopZeros(polymer);
}

void SpPrintPolymer(SparseMatrix * sp)
{
    printf("SparseMatrix [%ld x %ld] {\n", sp->nrows, sp->ncols);
    for(SpTriplet * it = sp->begin; it != sp->end; ++it)
    {
        printf("    (%c, %c): %lld\n", (char) it->row, (char) it->col, it->data);
    }
    printf("}\n");
}

solution_t SolvePart2(const bool is_test)
{
    FILE * file = GetFile(is_test, 14);

    Vector frequencies;
    NEW_VECTOR(frequencies);

    SparseMatrix polymer = ReadPolymerTemplateOptimized(file, &frequencies);
    SpMergeDuplicates(&polymer);

    InstructionVector instructions = ReadInstructions(file);
    InstructionQuickSort(instructions.begin, instructions.end);

    const size_t n_steps = 40;
    for (size_t step=0; step < n_steps; ++step) {
        NextStepOptimized(&polymer, &instructions, &frequencies);
    }


    long long int max = frequencies.begin[0];
    long long int min = frequencies.begin[0];

    for (long long int *it = frequencies.begin; it != frequencies.end; ++it) {
        if(*it > max) max = *it;
        if(*it!=0 && *it < min) min = *it;
        if(min == 0 && *it != 0) min = *it;
    }

    CLEAR(frequencies);
    CLEAR(instructions);
    ClearSparseMatrix(&polymer);

    fclose(file);
    return max - min;
}
