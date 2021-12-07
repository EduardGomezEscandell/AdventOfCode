#include "day7.h"
#include <stdio.h>

int main()
{
	const int test_1_solution = 37;
	const int test_2_solution = 168;


	printf("Running tests:\n");
	const int test_1 = SolvePart1(true);
	const char * fmt = "Wrong solution for test %d!\n> Expected: %ld\n> Obtained: %ld\n\n";
	if(test_1 != test_1_solution)
	{
		printf(fmt, 1, test_1_solution, test_1);
	}

	const int test_2 = SolvePart2(true);
	if(test_2 != test_2_solution)
	{
		printf(fmt, 2, test_2_solution, test_2);
	}

	if(test_1 == test_1_solution && test_2 == test_2_solution)
	{
		printf("All tests passed.\n\n");
	}

	printf("Running with real data:\n");
	printf("Part 1: %d\n", SolvePart1(false));
	printf("Part 2: %d\n", SolvePart2(false));
}
