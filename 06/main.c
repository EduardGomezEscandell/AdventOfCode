#include "day6.h"
#include <stdio.h>

int main()
{
	const unsigned long test_1_solution = 5934;
	const unsigned long test_2_solution = 26984457539;


	printf("Running tests:\n");
	const unsigned long test_1 = SolvePart1(true);
	const char * fmt = "Wrong solution for test %d!\n> Expected: %ld\n> Obtained: %ld\n\n";
	if(test_1 != test_1_solution)
	{
		printf(fmt, 1, test_1_solution, test_1);
	}

	const unsigned long test_2 = SolvePart2(true);
	if(test_2 != test_2_solution)
	{
		printf(fmt, 2, test_2_solution, test_2);
	}

	if(test_1 == test_1_solution && test_2 == test_2_solution)
	{
		printf("All tests passed.\n\n");
	}

	printf("Running with real data:\n");
	printf("Part 1: %ld\n", SolvePart1(false));
	printf("Part 2: %ld\n", SolvePart2(false));
}
