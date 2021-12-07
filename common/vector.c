#include "vector.h"
#include "math.h"

#include <stdio.h>
#include <stdlib.h>

#define GROWTH_FACTOR 1.6


int CharInString(char c, const char * const string)
{
	for(char const * it = string; *it != '\0'; ++it)
	{
		if(*it == c) return 1;
	}
	return 0;
}

Vector VectorFromString(
	char * const line,
	const char * const separators)
{
	char * begin = line;
	Vector v = EmptyVector();
	for(char * end = line; *end != '\0'; ++end)
	{
		if(CharInString(*end, separators))
		{
			*end = '\0';
			VectorPush(&v, atoi(begin));
			begin = end + 1;
		}
	}
	
	// Trailing int
	if(*begin != '\0')
	{
		VectorPush(&v, atoi(begin));
	}

	return v;
}


Vector EmptyVector()
{
	Vector s;
	s.begin = NULL;
	s.end = NULL;
	s.capacity = 0;
	return s;
}

Vector CreateVector(size_t size, const int value)
{
	Vector s = EmptyVector();
	VectorReserve(&s, size);
	s.end += size;
	for(int* it=s.begin; it != s.end; ++it)
	{
		*it = value;
	}
	return s;
}

void VectorCopy(Vector * out, const Vector in)
{
	VectorReserve(out, CONTAINER_LEN(in));
	
	int const * it = in.begin;
	for(out->end = out->begin; it != in.end; ++out->end, ++it)
	{
		*out->end = *it;
	}
}

void VectorReserve(Vector * vec, const size_t new_size)
{
	if(vec->capacity >= new_size) return;

	if(vec->begin == NULL)
	{
		vec->begin = (int *) malloc(new_size * sizeof(int));
		vec->end   = vec->begin;
		vec->capacity = new_size;
		return;
	}

	size_t len = CONTAINER_LEN(*vec);
	vec->begin = realloc(vec->begin, new_size * sizeof(int));
	vec->end   = vec->begin + len;
	vec->capacity = new_size;
}


void VectorGrow(Vector * vec, const size_t growth)
{
	const size_t min_size = CONTAINER_LEN(*vec) + growth;

	if(vec->capacity > min_size) return;

	if(vec->capacity == 0)
	{
		vec->begin = (int *) malloc(min_size * sizeof(int));
		vec->end   = vec->begin;
		vec->capacity = min_size;
		return;
	}

	size_t new_cap = vec->capacity * GROWTH_FACTOR;
	size_t new_size = MAX(new_cap, min_size);

	size_t len = CONTAINER_LEN(*vec);
	vec->begin = realloc(vec->begin, new_size * sizeof(int));
	vec->end   = vec->begin + len;
	vec->capacity = min_size;
}

void VectorShrink(Vector * vec)
{
	if(vec->begin == NULL) return;

	size_t new_size = CONTAINER_LEN(*vec);
	vec->begin = realloc(vec->begin, new_size * sizeof(int));
	vec->end   = vec->begin + new_size;
	vec->capacity = new_size;
}

/* 0 - Ok
 * 1 - capacity = 0 but begin != null
 * 2 - begin = null but end   != null
 * 3 - capacity < size
 */
// int VectorCheck(Vector vec)
// {
// 	if(vec.capacity == 0 && vec.begin != NULL) return 1;
// 	if(vec.begin == null && vec.end != NULL)   return 2;
// 	if(vec.capacity < CONTAINER_LEN(vec)) return 3;
// 	return 0;
// }

void VectorAppend(Vector * reciever, const Vector giver)
{
	VectorGrow(reciever, CONTAINER_LEN(giver));

	for(int const * it = giver.begin; it!=giver.end; ++it)
	{
		*reciever->end = *it;
		++reciever->end;
	}
}

void VectorPush(Vector * reciever, const int value)
{
	VectorGrow(reciever, 1);

	*reciever->end = value;
	++(reciever->end);
}

void VectorPop(Vector * vec)
{
	--vec->end;
}

void VectorPrint(const Vector vec)
{
	int* it=vec.begin;
	printf("[ ");
	
	while(it != vec.end)
	{
		printf("%d", *it);

		++it;
		if(it != vec.end)
		{
			printf(", ");
		} else {
			break;
		}
	};
	
	printf(" ]");
}

void VectorPrintLine(const Vector vec)
{
	VectorPrint(vec);
	printf("\n");
}


void VectorClear(Vector * vec)
{
	free(vec->begin);
	vec->begin = NULL;
	vec->end = NULL;
}

size_t VectorLen(Vector v)
{
	return CONTAINER_LEN(v);
}

int * MinEntry(const Vector v)
{
	int * min = v.begin;
	for(int * it = v.begin; it != v.end; ++it)
	{
		if(*it < *min) min = it;
	}
	return min;
}

int * MaxEntry(const Vector v)
{
	int * max = v.begin;
	for(int * it = v.begin; it != v.end; ++it)
	{
		if(*it > *max) max = it;
	}
	return max;
}