#include "vector.h"
#include "math.h"

#include <stdio.h>
#include <stdlib.h>

#define GROWTH_FACTOR 1.6

void QuickSort(Vector * vec)
{
	if(VectorLen(*vec) <= 1) return;

	vdata_type pivot = *vec->begin;

	Vector low = EmptyVector();
	Vector high = EmptyVector();
	
	for(vdata_type * it = vec->begin + 1; it != vec->end; ++it)
	{
		if(*it <= pivot)
		{
			VectorPush(&low, *it);
		}
		else
		{
			VectorPush(&high, *it);
		}
	}
	VectorPush(&high, pivot);

	QuickSort(&low);
	QuickSort(&high);

	VectorClear(vec);
	*vec = low;

	VectorAppend(vec, high);
	VectorClear(&high);
}

vdata_type CharInString(char c, const char * const string)
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
	
	// Trailing vdata_type
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

Vector CreateVector(size_t size, const vdata_type value)
{
	Vector s = EmptyVector();
	VectorReserve(&s, size);
	s.end += size;
	for(vdata_type* it=s.begin; it != s.end; ++it)
	{
		*it = value;
	}
	return s;
}

void VectorCopy(Vector * out, const Vector in)
{
	VectorReserve(out, CONTAINER_LEN(in));
	
	vdata_type const * it = in.begin;
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
		vec->begin = (vdata_type *) malloc(new_size * sizeof(vdata_type));
		vec->end   = vec->begin;
		vec->capacity = new_size;
		return;
	}

	size_t len = CONTAINER_LEN(*vec);
	vec->begin = realloc(vec->begin, new_size * sizeof(vdata_type));
	vec->end   = vec->begin + len;
	vec->capacity = new_size;
}


void VectorGrow(Vector * vec, const size_t growth)
{
	const size_t min_size = CONTAINER_LEN(*vec) + growth;

	if(vec->capacity > min_size) return;

	if(vec->capacity == 0)
	{
		vec->begin = (vdata_type *) malloc(min_size * sizeof(vdata_type));
		vec->end   = vec->begin;
		vec->capacity = min_size;
		return;
	}

	size_t new_cap = vec->capacity * GROWTH_FACTOR;
	size_t new_size = MAX(new_cap, min_size);

	size_t len = CONTAINER_LEN(*vec);
	vec->begin = realloc(vec->begin, new_size * sizeof(vdata_type));
	vec->end   = vec->begin + len;
	vec->capacity = min_size;
}

void VectorShrink(Vector * vec)
{
	if(vec->begin == NULL) return;

	size_t new_size = CONTAINER_LEN(*vec);
	vec->begin = realloc(vec->begin, new_size * sizeof(vdata_type));
	vec->end   = vec->begin + new_size;
	vec->capacity = new_size;
}

/* 0 - Ok
 * 1 - capacity = 0 but begin != null
 * 2 - begin = null but end   != null
 * 3 - capacity < size
 */
// vdata_type VectorCheck(Vector vec)
// {
// 	if(vec.capacity == 0 && vec.begin != NULL) return 1;
// 	if(vec.begin == null && vec.end != NULL)   return 2;
// 	if(vec.capacity < CONTAINER_LEN(vec)) return 3;
// 	return 0;
// }

void VectorAppend(Vector * reciever, const Vector giver)
{
	VectorGrow(reciever, CONTAINER_LEN(giver));

	for(vdata_type const * it = giver.begin; it!=giver.end; ++it)
	{
		*reciever->end = *it;
		++reciever->end;
	}
}

void VectorPush(Vector * reciever, const vdata_type value)
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
	vdata_type* it=vec.begin;
	printf("[ ");
	
	while(it != vec.end)
	{
		printf("%ld", *it);

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

vdata_type * MinEntry(const Vector v)
{
	vdata_type * min = v.begin;
	for(vdata_type * it = v.begin; it != v.end; ++it)
	{
		if(*it < *min) min = it;
	}
	return min;
}

vdata_type * MaxEntry(const Vector v)
{
	vdata_type * max = v.begin;
	for(vdata_type * it = v.begin; it != v.end; ++it)
	{
		if(*it > *max) max = it;
	}
	return max;
}