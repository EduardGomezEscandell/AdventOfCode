#ifndef DAY_03_H
#define DAY_03_H

#include <stdbool.h>
#include <stdio.h>

#include "common/testing.h"

typedef unsigned short binary;

binary ReadBinary(char const * const line);

void PopEntry(binary * list, size_t * size, const size_t i);

void ProcessCarbon(binary * carbon, size_t * n_carbon, const binary bit_mask);
void ProcessOxygen(binary * oxygen, size_t * n_oxygen, const binary bit_mask);


int SolvePart1(const bool is_test);
int SolvePart2(const bool is_test);

DECLARE_TEST(1);
DECLARE_TEST(2);

#endif
