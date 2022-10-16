#include "day10.h"
#include <stdio.h>

#include "common/defines.h"

int main(int argc, char ** argv)
{
	int run_tests=0;
	int run_solution=0;
	PARSE_INPUT(run_tests, run_solution);

	const size_t test_1_solution = 26397;
	const size_t test_2_solution = 288957;

	int return_value = EXIT_SUCCESS;
	
	if(run_tests)
	{
		printf("Running tests:\n");

		const solution_t test = Solve(true);	
		const size_t test_1 = test.corruption;
		const size_t test_2 = test.autocompletion;

		const char * fmt = "Wrong solution for test %d!\n> Expected: %ld\n> Obtained: %ld\n\n";
		if(test_1 != test_1_solution) {
			printf(fmt, 1, test_1_solution, test_1);
			return_value = EXIT_FAILURE;
		}
		
		if(test_2 != test_2_solution) {
			printf(fmt, 2, test_2_solution, test_2);
			return_value = EXIT_FAILURE;
		}
		
		if(test_1 == test_1_solution && test_2 == test_2_solution)
		{
			printf("All tests passed.\n\n");
		}
	}

	if(!run_solution) exit(return_value);
	
	printf("Running with real data:\n");
	
	Timer t1 = StartTimer();
	const solution_t solution = Solve(false);
	const size_t solution_1 = solution.corruption;
	const size_t solution_2 = solution.autocompletion;
	long t = StopTimer(&t1);

	printf("Part 1: %10ld \t (time:%10ld ns)\n", solution_1, t);	
	printf("Part 2: %10ld \t (time:%10ld ns)\n", solution_2, t);

	return return_value;
}
