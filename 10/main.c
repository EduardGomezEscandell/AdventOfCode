#include "day10.h"
#include <stdio.h>

#include "common/timing.h"

int main()
{
	const size_t test_1_solution = 26397;
	const size_t test_2_solution = 288957;

	printf("Running tests:\n");
	
	const solution_t test = Solve(true);	
	const size_t test_1 = test.corruption;
	const size_t test_2 = test.autocompletion;

	const char * fmt = "Wrong solution for test %d!\n> Expected: %ld\n> Obtained: %ld\n\n";
	if(test_1 != test_1_solution)
	{
		printf(fmt, 1, test_1_solution, test_1);
	}
	
	if(test_2 != test_2_solution)
	{
		printf(fmt, 2, test_2_solution, test_2);
	}
	
	if(test_1 == test_1_solution && test_2 == test_2_solution)
	{
		printf("All tests passed.\n\n");
	}
	
	printf("Running with real data:\n");
	
	Timer t1 = StartTimer();
	const solution_t solution = Solve(false);
	const size_t solution_1 = solution.corruption;
	const size_t solution_2 = solution.autocompletion;
	long t = StopTimer(&t1);

	printf("Part 1: %10ld \t (time:%10ld ns)\n", solution_1, t);	
	printf("Part 2: %10ld \t (time:%10ld ns)\n", solution_2, t);
}
