#include "day23.h"

#include "routing.h"
#include "gamestate.h"
#include "dfs.h"

#include "common/vector.h"
#include "common/math.h"

#include <stdlib.h>
#include <stdio.h>

solution_t Solve(ProblemData const * problem_data)
{
    GameState initial_gamestate = ReadGamestate(problem_data);

    RoutingTable routing = BuildRoutingTable(problem_data);

    return DFS(&initial_gamestate, &routing, problem_data);
}

solution_t SolvePart1(const bool is_test)
{
    const ProblemData problem_data = FillProblemData(is_test, 1);
    return Solve(&problem_data);
}

solution_t SolvePart2(const bool is_test)
{
    const ProblemData problem_data = FillProblemData(is_test, 2);
    return Solve(&problem_data);
}

DEFINE_TEST(1, 46)
DEFINE_TEST(2, 114)
