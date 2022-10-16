#ifndef DAY_02_H
#define DAY_02_H

#include <stdbool.h>
#include <stdio.h>

#include "common/testing.h"

typedef enum {UP, DOWN, FORWARD} direction;

typedef struct{
	int units;
	direction direction;
} Command;

Command ReadCommand(char * line);
int SolvePart1(bool is_test);
int SolvePart2(bool is_test);

DECLARE_TEST(1);
DECLARE_TEST(2);

#endif
