#include "common/defines.h"
#include "day19.h"

#include <stdlib.h>

int main(int argc, char **argv) {
    int run_tests = 0;
    int run_solution = 0;
    
    PARSE_INPUT(run_tests, run_solution);

    int result = 0;
    if (run_tests)
    {
        printf("Running tests:\n\n");
        const char *fmt = "Wrong solution for test %d!\n> Expected: %ld\n> Obtained: %ld\n\n";
        
        printf("Running test\n");
        fflush(stdout);
        
        const day19_test_result test = Test();

        if (test.success[0])
            printf("Test 1 successful\n\n");
        else
            printf(fmt, 1, test.expected.part_1, test.obtained.part_1);

        if (test.success[1])
            printf("Test 2 successful\n\n");
        else
            printf(fmt, 2, test.expected.part_2, test.obtained.part_2);
        
        result = test.success[0] && test.success[1] ? EXIT_SUCCESS : EXIT_FAILURE;
    }


    if (!run_solution) return result;

    printf("Running with real data\n\n");
    printf("Running Part 1\n");
    fflush(stdout);
    Timer t1 = StartTimer();
    solution_t solution = Solve(false);
    long t = StopTimer(&t1);
    printf("Solution 1: %ld\n", solution.part_1);
    printf("Solution 2: %ld\n", solution.part_2);
    printf("Time:         %ld ns\n\n", t);
    fflush(stdout);

    return result;
}
