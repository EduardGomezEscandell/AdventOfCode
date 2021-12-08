#ifndef DAY_08_H
#define DAY_08_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct 
{
	char * codes[10];
} Decoder;


#include "day8.h"
#include "common/file_utils.h"
#include <string.h>

// String querrying
char * SkipToOutput(char * const line);
char * FindWordWithLength(char * line, const size_t len);

// Sets
unsigned int * Find(unsigned int array[4], unsigned int val);
char FindMismatch(char * word1, char * word2);
bool Contains(char * container, char * contained);
char IncludeExclude(char * common1, char * common2, char * exclude);

// Decoder
void PrintDecoder(Decoder * d);
void ClearDecoder(Decoder * d);
Decoder GetDecoder(char * line);
char * CopyWord(char * word_begin);

int DecodeWord(Decoder *dec, char * begin);
int Decode(Decoder * dec, char * line);

// Solving
typedef int solution_t;
solution_t SolvePart1(const bool is_test);
solution_t SolvePart2(const bool is_test);

#endif
