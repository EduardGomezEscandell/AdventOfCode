#ifndef TESTING_INCLUDED_H
#define TESTING_INCLUDED_H

typedef struct
{
    long int expected;
    long int obtained;
    int success;
} TestResult;

#define DECLARE_TEST(DEC_TEST_ID) TestResult TestPart ## DEC_TEST_ID()

#define DEFINE_TEST(DEF_TEST_ID, expected_result)                             \
TestResult TestPart ## DEF_TEST_ID()                                          \
{                                                                             \
    TestResult t;                                                             \
    t.expected = expected_result;                                             \
    t.obtained = SolvePart ## DEF_TEST_ID(true);                              \
    t.success = (t.expected == t.obtained);                                   \
    return t;                                                                 \
}

#define RUN_TESTS(result) do {                                                \
printf("Running tests:\n\n");                                                 \
const char * fmt                                                              \
    = "Wrong solution for test %d!\n> Expected: %ld\n> Obtained: %ld\n\n";    \
printf("Running test 1\n"); fflush(stdout);                                   \
const TestResult test_1 = TestPart1(true);                                    \
if(test_1.success)                                                            \
    printf("Test 1 successful\n\n");                                          \
else                                                                          \
    printf(fmt, 1, test_1.expected, test_1.obtained);                         \
printf("Running test 2\n"); fflush(stdout);                                   \
const TestResult test_2 = TestPart2(true);                                    \
if(test_2.success)                                                            \
    printf("Test 2 successful\n\n");                                          \
else                                                                          \
    printf(fmt, 2, test_2.expected, test_2.obtained);                         \
result = test_1.success && test_2.success ? EXIT_SUCCESS : EXIT_FAILURE;      \
} while(0)


#endif
