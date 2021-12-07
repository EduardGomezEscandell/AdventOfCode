#include "day7.h"
#include "common/file_utils.h"
#include "common/vector.h"

char * ReadSingleLine(FILE * file)
{
	char * line = NULL;
	size_t len = 0;
	ssize_t read = 0;

	if((read = getline(&line, &len, file)) == -1)
	{
		fprintf(stderr, "Failed to read file\n");
		exit(EXIT_FAILURE);
	}
	fclose(file);

	return line;
}

int ComputeFuelExpense(const Vector v, int destination, int(*fuel_formula)(const int, const int))
{
	unsigned int cost = 0;
	for(int * it=v.begin; it != v.end; ++it)
	{
		cost += fuel_formula(*it, destination);
	}
	return cost;
}

int UniformCostExpense(const int source, const int destination)
{
	return abs(destination - source);
}

int QuadraticCostExpense(const int source, const int destination)
{
	const int n = abs(destination - source);
	return n*(n+1)/2;
}


int SolvePart1(const bool is_test)
{
	FILE* file = GetFile(is_test, 7);
	char * line = ReadSingleLine(file);

	Vector v = VectorFromString(line, ",\n");
	free(line);

	int * min_entry = MinEntry(v);
	int min = ComputeFuelExpense(v, *min_entry, &UniformCostExpense);

	for(int i=*min_entry+1; i <= *MaxEntry(v); ++i)
	{
		int expense = ComputeFuelExpense(v, i, &UniformCostExpense);
		if(expense < min)
		{
			min = expense;
		}
	}

	VectorClear(&v);
	return min;
}

int SolvePart2(const bool is_test)
{
	FILE* file = GetFile(is_test, 7);
	char * line = ReadSingleLine(file);

	Vector v = VectorFromString(line, ",\n");
	free(line);

	int * min_entry = MinEntry(v);
	int min = ComputeFuelExpense(v, *min_entry, &QuadraticCostExpense);

	for(int i=*min_entry+1; i <= *MaxEntry(v); ++i)
	{
		int expense = ComputeFuelExpense(v, i, &QuadraticCostExpense);
		if(expense < min)
		{
			min = expense;
		}
	}

	VectorClear(&v);
	return min;
}
