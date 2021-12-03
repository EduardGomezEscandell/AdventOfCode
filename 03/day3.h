#ifndef DAY_03_H
#define DAY_03_H

#include <stdbool.h>
#include <stdio.h>

FILE * GetFile(const bool is_test);

unsigned int ReadBinary(char const * const line);

void PopEntry(char ** list, size_t * size, const size_t i);

void ProcessCarbon(char ** carbon, size_t * n_carbon, const size_t digit);
void ProcessOxygen(char ** oxygen, size_t * n_oxygen, const size_t digit);


int SolvePart1(const bool is_test);
int SolvePart2(const bool is_test);


#endif
