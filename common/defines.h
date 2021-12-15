#ifndef DEFINES_INCLUDED_H
#define DEFINES_INCLUDED_H

#include "common/timing.h"
#include "common/testing.h"
#include <stdlib.h>
#include <string.h>

#define PARSE_INPUT(test_flag, solution_flag) do {                        \
for(int i=1; i<argc; ++i) {                                               \
    if(!test_flag && strcmp(argv[i], "-t") == 0)                          \
        test_flag = true;                                                 \
    else if(!solution_flag && strcmp(argv[i], "-s") == 0)                 \
        solution_flag = true;                                             \
}                                                                         \
}while(0)

#define AOC_MAIN                                                              \
int main(int argc, char ** argv) {                                            \
    int run_tests = 0;                                                        \
    int run_solution = 0;                                                     \
    PARSE_INPUT(run_tests, run_solution);                                     \
                                                                              \
    int result = EXIT_SUCCESS;                                                \
    if(run_tests) RUN_TESTS(result);                                          \
                                                                              \
    if(!run_solution) return result;                                          \
    printf("Running with real data\n\n");                                     \
    printf("Running Part 1\n"); fflush(stdout);                               \
    Timer t1 = StartTimer();                                                  \
    long long int solution_1 = SolvePart1(false);                             \
    long t = StopTimer(&t1);                                                  \
    printf("Solution: %lld\n", solution_1);                                   \
    printf("Time:     %ld ns\n\n", t); fflush(stdout);                        \
                                                                              \
    printf("Running Part 2\n"); fflush(stdout);                               \
    Timer t2 = StartTimer();                                                  \
    long long int solution_2 = SolvePart2(false);                             \
    t = StopTimer(&t2);                                                       \
    printf("Solution: %lld\n", solution_2);                                   \
    printf("Time:     %ld ns\n\n", t); fflush(stdout);                        \
    return result;                                                            \
}



#endif