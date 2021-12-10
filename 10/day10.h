#ifndef DAY_10_H
#define DAY_10_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct
{
	size_t * base;     // base of stack
	size_t * top;      // past-the-end pointer
	size_t capacity; // Allocated memory
} Stack;

Stack EmptyStack();
void ClearStack(Stack * s);
void Push(Stack * stack, const size_t c);
size_t Pop(Stack * stack);
void PrintStack(Stack s, const char * line);


typedef struct 
{
	size_t corruption;
	size_t autocompletion;
} Score;

void PrintCorruptionError(const size_t row, const size_t col, const size_t i_opener, const char read, const char * line);
void PrintWrongCharError(const size_t row, const size_t col, const char read, const char * line);

char Closer(char c);
int ComputeScore(char c);
size_t ComputeLineAutocompletionScore(Stack stack, const char * line);
Score ComputeLineScore(const char * line, const size_t line_id);
Score Solve(const bool is_test);

// Solving
typedef Score solution_t;
solution_t Solve(const bool is_test);

#endif
