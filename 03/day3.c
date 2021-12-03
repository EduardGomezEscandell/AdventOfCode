#include "day3.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* GetFile(const bool is_test)
{
	char* filename;
	if(is_test)
	{
		filename = "data/03/test.txt";
	} else {
		filename = "data/03/data.txt";
	}

	FILE* file = fopen(filename, "r");

	if (file == NULL) exit(EXIT_FAILURE);

	return file;
}

void UpdateMostCommonList(const size_t n_digits, int * count, const char * const line)
{
	for(size_t i=0; i<n_digits; ++i)
	{
		switch (line[i]) {
		case '0': count[i] -= 1; continue;
		case '1': count[i] += 1; continue;
		default: 
			fprintf(stderr, "Unexpected character in line: %s", line);
			exit(EXIT_FAILURE);
		}
	}	
}


int SolvePart1(const bool is_test)
{
	FILE * file = GetFile(is_test);

	char * line = NULL;
	size_t len = 0;
	ssize_t read = 0;

	const size_t n_digits = is_test ? 5 : 12;

	int * count = calloc(n_digits, sizeof(int));

	while((read = getline(&line, &len, file)) != -1)
	{
		UpdateMostCommonList(n_digits, count, line);
	}

	int gamma = 0;
	int epsilon = 0;

	for(unsigned int i=0; i<n_digits; ++i)
	{
		gamma   = gamma*2   + (count[i] > 0);
		epsilon = epsilon*2 + (count[i] < 0);
	}

	free(count);
	free(line);
	fclose(file);

	return gamma * epsilon;	
}


void PopEntry(char ** list, size_t * size, const size_t i)
{
	free(list[i]);
	*size = *size - 1;

	list[i]     = list[*size];
	list[*size] = NULL;
}

void ProcessOxygen(
	char ** oxygen,
	size_t * n_oxygen,
	const size_t digit)
{
	int count = 0;
	for(size_t i = 0; i < *n_oxygen; ++i)
	{
		count += oxygen[i][digit] == '1' ? 1 : -1;
	}
	char most_common = count >= 0 ? '1' : '0';

	for(ssize_t i = *n_oxygen - 1; i >= 0 && *n_oxygen > 1; --i)
	{
		if(oxygen[i][digit] != most_common)
		{
			PopEntry(oxygen, n_oxygen, i);
		}
	}
}

void ProcessCarbon(
	char ** carbon,
	size_t * n_carbon,
	const size_t digit)
{
	int count = 0;
	for(size_t i = 0; i < *n_carbon; ++i)
	{
		count += carbon[i][digit] == '1' ? 1 : -1;
	}
	char most_common = count >= 0 ? '1' : '0';

	for(int i = *n_carbon - 1; i >= 0 && *n_carbon > 1; --i)
	{
		if(carbon[i][digit] == most_common)
		{
			PopEntry(carbon, n_carbon, i);
		}
	}
}

unsigned int ReadBinary(char const * const line)
{
	unsigned int bin = 0;
	for(char const * it = line; *it != '\0'; ++it)
	{
		switch (*it) {
			case '0': bin = bin << 1;		break;
			case '1': bin = (bin << 1) + 1; break;
		}
	}
	return bin;
}

int SolvePart2(const bool is_test)
{
	FILE* file = GetFile(is_test);

	char * line = NULL;
	size_t len = 0;
	ssize_t read = 0;

	const size_t n_digits = is_test ? 5 : 12;
	const size_t n_entries = is_test ? 12 : 1000;

	char ** oxygen = malloc(n_entries * sizeof(char *));
	char ** carbon = malloc(n_entries * sizeof(char *));
	
	size_t i = 0;
	while((read = getline(&line, &len, file)) != -1)
	{
		oxygen[i] = malloc((n_digits+2) * sizeof(char));
		carbon[i] = malloc((n_digits+2) * sizeof(char));
		strcpy(oxygen[i], line);
		strcpy(carbon[i], line);
		++i;
	}

	// Oxygen
	size_t n_oxygen = n_entries;
	for(size_t i = 0; i < n_digits && n_oxygen > 1; ++i)
	{
		ProcessOxygen(oxygen, &n_oxygen, i);
	}

	// Carbon
	size_t n_carbon = n_entries;
	for(size_t i = 0; i < n_digits && n_carbon > 1; ++i)
	{
		ProcessCarbon(carbon, &n_carbon, i);
	}

	const int result = ReadBinary(*oxygen) * ReadBinary(*carbon);

	for(size_t i = 0; i<n_oxygen; ++i) free(oxygen[i]);
	for(size_t i = 0; i<n_carbon; ++i) free(carbon[i]);
	free(oxygen);
	free(carbon);
	free(line);
	fclose(file);

	return result;
}
