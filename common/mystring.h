#ifndef MY_STRING_INCLUDED_H
#define MY_STRING_INCLUDED_H

#include <stdlib.h>

typedef struct
{
	char * begin;
	char * end;
	size_t capacity;
} String;


// Construction
String CreateString(const char*);
String EmptyString();
void StringCopy(String *, const String);

// Memory
void StringReserve(String *, const size_t);
void StringClear(String);

// Manipulation
void StringAppend(String *, const String);
void StringAppendC(String *, const char *);

// Output
void StringPrint(const String);
void StringPrintLine(const String);

// Conversion
char * StringToC(const String);

#endif