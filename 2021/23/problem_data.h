#ifndef PROBLEM_DATA_DAY_23_H
#define PROBLEM_DATA_DAY_23_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>


// Typedefs
typedef uint_least8_t location_t;
typedef uint_least32_t route_t;
typedef int_least32_t player_t;
typedef int cost_t;

#define INF_COST -1

#define NOL     ((location_t) 31) /* No location*/
#define MAXLOCS ((location_t) 23) /* 23 nodes at most */

#define MAX_PLAYERS 16
#define NO_PLAYER 0XFF


typedef struct 
{
    player_t n_players;
    location_t n_locations;
    int part;
    bool is_test;
} ProblemData;

static inline ProblemData FillProblemData(bool is_test, int part)
{
    ProblemData problem_data;
    problem_data.part = part;
    problem_data.is_test = is_test;
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

#endif
