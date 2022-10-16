#ifndef DAY_12_H
#define DAY_12_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "common/testing.h"

typedef enum {START, END, SMALL, BIG} CaveType;

typedef struct cave
{
	size_t id;
	unsigned int n_visits;
	CaveType type;
	struct cave ** neighbours;
	size_t n_neighbours;
} Cave;

typedef struct
{
	Cave * begin;
	Cave * end;
	size_t capacity;
} CaveSystem;


// Hashing
size_t HashString(const char * const begin, const char * const end);
CaveType Classify(const char * const begin, const size_t hash);
size_t StartId();
size_t EndId();

// Cave memory management
Cave NewCave(const size_t id, CaveType type);
void PrintCave(const Cave * const cave);
void ClearCave(Cave * cave);
void SetAsNeighbours(Cave * cave1, Cave * cave2);

// Cave system memory management
CaveSystem NewCaveSystem();
void PrintCaveSystem(const CaveSystem * const cs);
void ClearCaveSystem(CaveSystem * cs);
Cave * FindCave(CaveSystem * cs, const size_t id_lookup, const CaveType type);
Cave * PushCave(CaveSystem * cs, const size_t id, const CaveType type);

// Reading from input
void ReadCavesInLine(size_t ids[2], CaveType types[2], char * line);
CaveSystem ReadCaves(FILE * file);

// Exploring
size_t CountPaths(CaveSystem *cs, Cave * start, unsigned int max_small_visits);

// Solving
typedef int solution_t;
solution_t Solve(const bool is_test, const unsigned int max_small_visits);
solution_t SolvePart1(const bool is_test);
solution_t SolvePart2(const bool is_test);

DECLARE_TEST(1);
DECLARE_TEST(2);

#endif
