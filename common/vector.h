#ifndef VECTOR_INCLUDED_H
#define VECTOR_INCLUDED_H

#include <stdlib.h>

typedef struct
{
	int * begin;
	int * end;
	size_t capacity;
} Vector;

// Construction
Vector EmptyVector();
Vector CreateVector(const size_t, const int);
void VectorCopy(Vector *, const Vector);

// Memory
void VectorReserve(Vector *, const size_t);
void VectorGrow(Vector *, const size_t);
void VectorShrink(Vector * vec);
void VectorClear(Vector);

// Manipulation
void VectorAppend(Vector *, const Vector);
void VectorPush(Vector *, const int);
void VectorPop(Vector *);

// Output
void VectorPrint(const Vector);
void VectorPrintLine(const Vector);


#endif
