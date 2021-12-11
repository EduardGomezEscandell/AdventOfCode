#ifndef DAY_09_H
#define DAY_09_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>


typedef struct {
	int ** z;
	size_t ** basin;
	size_t x_span;
	size_t y_span;
} HeightMap;

typedef struct {
	size_t id;
	size_t size;
} Basin;

int * StringToInts(char * line, const size_t len);

HeightMap ReadHeightMap(FILE * file);
void ClearHeightMap(HeightMap * hmap);

void PrintBasins(HeightMap * hmap);
void PrintHeightMap(HeightMap * hmap);

int GetDangerScore(HeightMap * hmap, const size_t x, const size_t y);

size_t ExploreDownstream(HeightMap * hmap, size_t x, size_t y);
void LabelBasins(HeightMap * hmap);

void Insert(Basin * list, size_t n_sorted, size_t i);
void FindLargestBasins(Basin largest_basins[3], const HeightMap * const hmap, const size_t n_basins);

// Solving
typedef int solution_t;
solution_t SolvePart1(const bool is_test);
solution_t SolvePart2(const bool is_test);

#endif
