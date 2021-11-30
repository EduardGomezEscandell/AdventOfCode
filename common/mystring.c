#include "mystring.h"
#include "math.h"

#include <stdio.h>
#include <string.h>

String CreateString(const char* input)
{
	String s;
	s.capacity = strlen(input);
	s.begin = (char *) malloc(s.capacity * sizeof(char));
	s.end=s.begin;

	for(size_t i=0; input[i]!='\0'; ++i, ++s.end)
	{
		*s.end = input[i];
	}
	return s;
}

String EmptyString()
{
	String s;
	s.begin = NULL;
	s.end = NULL;
	s.capacity = 0;
}

void StringCopy(String * out, const String in)
{
	StringReserve(out, CONTAINER_LEN(in));
	
	char const * it = in.begin;
	for(out->end = out->begin; it != in.end; ++out->end, ++it)
	{
		*out->end = *it;
	}
}

void StringReserve(String * str, const size_t new_size)
{
	if(str->capacity >= new_size) return;

	if(str->begin == NULL)
	{
		str->begin = (char *) malloc(new_size * sizeof(char));
		str->end   = str->begin;
		str->capacity = new_size;
		return;
	}

	size_t len = CONTAINER_LEN(*str);
	str->begin = realloc(str->begin, new_size);
	str->end   = str->begin + len;
	str->capacity = new_size;
}

void StringAppend(String * reciever, const String giver)
{
	const size_t len_0 = CONTAINER_LEN(*reciever);
	const size_t len_1 = CONTAINER_LEN(giver);
	StringReserve(reciever, len_0 + len_1);

	for(char const * it = giver.begin; it!=giver.end; ++it)
	{
		*reciever->end = *it;
		++reciever->end;
	}
}

void StringAppendC(String * reciever, const char * giver)
{
	const size_t len_0 = CONTAINER_LEN(*reciever);
	const size_t len_1 = strlen(giver);
	StringReserve(reciever, len_0 + len_1);

	for(char const * it = giver; *it!='\0'; ++it)
	{
		*reciever->end = *it;
		++reciever->end;
	}
}

void StringPrint(const String str)
{
	printf("%.*s", (int) CONTAINER_LEN(str), str.begin);
}

void StringPrintLine(const String str)
{
	printf("%.*s\n", (int) CONTAINER_LEN(str), str.begin);
}

char * StringToC(const String str)
{
	size_t len = CONTAINER_LEN(str) + 1; // null terminator
	char * ret = (char *) malloc(len * sizeof(char));
	char* out = ret;
	for(char const * in = str.begin; in!=str.end; ++out)
	{
		*out = *(in++);
	}
	out = '\0';
	return ret;
}

void StringClear(String str)
{
	free(str.begin);
	str.begin = NULL;
	str.end = NULL;
}