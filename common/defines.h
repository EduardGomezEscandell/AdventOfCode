#ifndef DEFINES_INCLUDED_H
#define DEFINES_INCLUDED_H

#define MAIN(format)                                                          \
printf("Running tests:\n");                                                   \
const solution_t test_1 = SolvePart1(true);                                   \
const char * fmt = "Wrong solution for test %d!\n> Expected: %ld\n> Obtained: %ld\n\n"; \
if(test_1 != test_1_solution)                                                 \
{                                                                             \
	printf(fmt, 1, test_1_solution, test_1);                                  \
}                                                                             \
                                                                              \
const solution_t test_2 = SolvePart2(true);                                   \
if(test_2 != test_2_solution)                                                 \
{                                                                             \
	printf(fmt, 2, test_2_solution, test_2);                                  \
}                                                                             \
                                                                              \
if(test_1 == test_1_solution && test_2 == test_2_solution)                    \
{                                                                             \
	printf("All tests passed.\n\n");                                          \
}                                                                             \
                                                                              \
printf("Running with real data:\n");                                          \
                                                                              \
Timer t1 = StartTimer();                                                      \
solution_t solution_1 = SolvePart1(false);                                    \
long t = StopTimer(&t1);                                                      \
printf("Part 1: "format" \t (time:%10ld ns)\n", solution_1, t);                   \
                                                                              \
Timer t2 = StartTimer();                                                      \
solution_t solution_2 = SolvePart2(false);                                    \
t = StopTimer(&t2);                                                           \
                                                                              \
printf("Part 2: "format" \t (time:%10ld ns)\n", solution_2, t);


#endif