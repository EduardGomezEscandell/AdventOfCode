#ifndef DAY_02_H
#define DAY_02_H

#include <stdbool.h>
#include <stdio.h>

typedef enum {UP, DOWN, FORWARD} direction;

typedef struct{
	int units;
	direction direction;
} Command;

Command ReadCommand(char * line);
int SolvePart1(bool is_test);
int SolvePart2(bool is_test);


#endif
