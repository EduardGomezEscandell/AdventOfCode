#ifndef VECTOR_INCLUDED_H
#define VECTOR_INCLUDED_H

#include <stdlib.h>

typedef long int vdata_type;

typedef struct
{
	vdata_type * begin;
	vdata_type * end;
	size_t capacity;
} Vector;


// Construction
Vector EmptyVector();
Vector CreateVector(const size_t, const vdata_type);
Vector VectorFromString(
	char * const line,
	const char * const separators);
void VectorCopy(Vector *, const Vector);

// Memory
void VectorReserve(Vector *, const size_t);
void VectorGrow(Vector *, const size_t);
void VectorShrink(Vector * vec);
void VectorClear(Vector *);
size_t VectorLen(Vector v);

// Manipulation
void VectorAppend(Vector *, const Vector);
void VectorPush(Vector *, const vdata_type);
void VectorPop(Vector *);

void QuickSort(Vector * vec);

// Querry
vdata_type * MinEntry(const Vector v);
vdata_type * MaxEntry(const Vector v);

// Output
void VectorPrint(const Vector);
void VectorPrintLine(const Vector);


#endif
