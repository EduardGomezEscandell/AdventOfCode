#include "day7.h"
#include "common/file_utils.h"

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
	for(long * it=v.begin; it != v.end; ++it)
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


// Fuel_formula must have a single local minimum which is also the global minimum
int OptimizeFuelExpense(const Vector v, int(*fuel_formula)(const int, const int))
{
	// Computing average position
	int avg = 0;
	size_t n = 0;
	for(long *it=v.begin; it != v.end; ++it)	
	{
		avg += *it;
		++n;
	}
	avg /= n;

	// Starting search at average position
	int x = avg;
	int fuel_p = ComputeFuelExpense(v, x-1, fuel_formula);
	int fuel_x = ComputeFuelExpense(v, x, fuel_formula);
	int fuel_n = ComputeFuelExpense(v, x+1, fuel_formula);

	// Edge case: average is minimum
	bool minimum = fuel_p > fuel_x && fuel_n > fuel_x;
	if(minimum) return fuel_x;

	// Else: move towards minimum
	int dx = -1;
	if(fuel_p > fuel_x) dx = 1;

	while (!minimum)
	{
		x += dx;
		fuel_p = fuel_x;                                      // fuel @ x-dx
		fuel_x = fuel_n;                                      // fuel @ x
		fuel_n = ComputeFuelExpense(v, x + dx, fuel_formula); // fuel @ x+dx
		minimum = fuel_p > fuel_x && fuel_n > fuel_x;
	}

	return fuel_x;
}


int Solve(const bool is_test, int(*fuel_formula)(const int, const int))
{
	FILE* file = GetFile(is_test, 7);
	char * line = ReadSingleLine(file);

	Vector v;
	NEW_VECTOR(v);
	STRING_TO_VECTOR(v, line, ",", long int);
	free(line);

	int min = OptimizeFuelExpense(v, fuel_formula);

	CLEAR(v);
	return min;
}

int SolvePart1(const bool is_test)
{
	return Solve(is_test, UniformCostExpense);
}

int SolvePart2(const bool is_test)
{
	return Solve(is_test, QuadraticCostExpense);
}
