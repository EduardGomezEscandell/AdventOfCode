#ifndef DAY_15_H
#define DAY_15_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "common/testing.h"
#include "common/vector.h"
#include "common/matrix.h"

typedef short Flag;

typedef struct graph_node_{
	// Graph stuff
	size_t id;
	struct graph_node_ * prev;
	struct graph_node_ * neighbours[4];
	// Dijkstra stuff
	unsigned int danger;
	int distance;
	bool visited;
} Node;

TEMPLATE_VECTOR(Node) Graph;
TEMPLATE_VECTOR(Node*) Queue;


void PrintNode(Node * node);

int CompareDistances(const Node * const A, const Node * const B);

size_t GetId(const size_t row, const size_t col, const size_t ncols);
Graph CreateGraph(Matrix * data, const size_t row_folds, const size_t col_folds);

Node ** MinEntry(Node ** begin, Node ** end);
long long int Distance(const Node * const A, const Node * const B);
void Dijkstra(Node * begin, Node * end, Node * start, Node * target);


// Solving
typedef int solution_t;
solution_t Solve(const bool is_test,const size_t row_folds, const size_t col_folds);
solution_t SolvePart1(const bool is_test);
solution_t SolvePart2(const bool is_test);

DECLARE_TEST(1);
DECLARE_TEST(2);

#endif
