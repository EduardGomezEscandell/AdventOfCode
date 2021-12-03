#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "day3.h"
#include "common/file_utils.h"

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
	FILE * file = GetFile(is_test, 3);

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


void PopEntry(binary * list, size_t * size, const size_t i)
{
	*size = *size - 1;
	list[i]     = list[*size];
}

void ProcessOxygen(
	binary * oxygen,
	size_t * n_oxygen,
	const binary bit_mask)
{
	int count = 0;
	for(size_t i = 0; i < *n_oxygen; ++i)
	{
		const bool bit = oxygen[i] & bit_mask;
		count += bit ? 1 : -1;
	}

	const bool most_common = count >= 0 ? 1 : 0;

	for(ssize_t i = *n_oxygen - 1; i >= 0; --i)
	{
		const bool bit = oxygen[i] & bit_mask;
		if(bit != most_common)
		{
			PopEntry(oxygen, n_oxygen, i);

			if(*n_oxygen == 1) return;
		}
	}
}

void ProcessCarbon(
	binary * carbon,
	size_t * n_carbon,
	const binary bit_mask)
{
	int count = 0;
	for(size_t i = 0; i < *n_carbon; ++i)
	{
		const bool bit = carbon[i] & bit_mask;
		count += bit ? 1 : -1;
	}
	const bool most_common = count >= 0 ? 1 : 0;

	for(ssize_t i = *n_carbon - 1; i >= 0; --i)
	{
		const bool bit = (carbon[i] & bit_mask) > 0;
		if(bit == most_common)
		{
			PopEntry(carbon, n_carbon, i);

			if(*n_carbon == 1) return;
		}
	}
}

binary ReadBinary(char const * const line)
{
	binary bin = 0;
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
	FILE* file = GetFile(is_test, 3);

	char * line = NULL;
	size_t len = 0;
	ssize_t read = 0;

	const size_t n_digits = is_test ? 5 : 12;
	const size_t n_entries = is_test ? 12 : 1000;

	binary * oxygen = malloc(n_entries * sizeof(binary));
	binary * carbon = malloc(n_entries * sizeof(binary));
	
	size_t i = 0;
	while((read = getline(&line, &len, file)) != -1)
	{
		oxygen[i] = ReadBinary(line);
		carbon[i] = oxygen[i];
		++i;
	}

	// Oxygen
	size_t n_oxygen = n_entries;

	for(binary bit_mask = 1 << (n_digits-1);
		bit_mask != 0;
		bit_mask = bit_mask >> 1)
	{
		ProcessOxygen(oxygen, &n_oxygen, bit_mask);
		if(n_oxygen == 1) break;
	}

	// Carbon
	size_t n_carbon = n_entries;

	for(binary bit_mask = 1 << (n_digits-1); 
		bit_mask != 0;
		bit_mask = bit_mask >> 1)
	{
		ProcessCarbon(carbon, &n_carbon, bit_mask);
		if(n_carbon == 1) break;
	}

	const int result = oxygen[0] * carbon[0];

	free(oxygen);
	free(carbon);
	free(line);
	fclose(file);

	return result;
}
