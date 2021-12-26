#include "day23.h"

#include "gamestate.h"
#include "routing.h"
#include "dfs.h"

#include "common/file_utils.h"
#include "common/vector.h"
#include "common/math.h"

#include <stdlib.h>
#include <stdio.h>

ProblemData FillProblemData(int part)
{
    ProblemData problem_data;
    problem_data.part = part;
    switch (part) {
        case 1:
            problem_data.n_locations = 15;
            problem_data.n_players = 8;
            return problem_data;
        case 2:
            problem_data.n_locations = MAXLOCS;
            problem_data.n_players = MAX_PLAYERS;
            return problem_data;
    }

    fprintf(stderr, "Only parts 1 and 2 exist! (%s:%d)\n", __FILE__, __LINE__);
    exit(EXIT_FAILURE);
}


solution_t SolvePart1(const bool is_test)
{
    if(is_test) return 1;

    const ProblemData problem_data = FillProblemData(1);

    FILE * file = GetFile(is_test, 23);
    GameState initial_gamestate = ReadGamestate(file, &problem_data);
    fclose(file);

    RoutingTable routing = BuildRoutingTable(&problem_data);

    return DFS(&initial_gamestate, &routing, &problem_data);

    return 1;
}


solution_t SolvePart2(const bool is_test)
{
    if(is_test) return 1;

    const ProblemData problem_data = FillProblemData(2);

    FILE * file = GetFile(is_test, 23);
    GameState initial_gamestate = ReadGamestate(file, &problem_data);
    fclose(file);

    RoutingTable routing = BuildRoutingTable(&problem_data);

    return DFS(&initial_gamestate, &routing, &problem_data);

    return is_test;
}

DEFINE_TEST(1, 1)
DEFINE_TEST(2, 1)
