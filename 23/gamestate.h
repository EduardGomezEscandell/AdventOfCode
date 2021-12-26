#ifndef DAY_23_GAMESTATE_H
#define DAY_23_GAMESTATE_H

#include "common/vector.h"
#include "routing.h"
#include "problem_data.h"

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>


typedef struct
{
    location_t locations[MAX_PLAYERS];
    bool moveflags[MAX_PLAYERS];
} GameState;

typedef struct 
{
    cost_t cost;
    GameState state;
} Continuation;

TEMPLATE_VECTOR(Continuation) ContinuationArray;

void GetLine(char ** line, FILE * file);
GameState ReadGamestate(ProblemData const * pdata);

location_t GetLocation(GameState const * gs, player_t player);

void PrettyPrintGamestate(GameState const * gs, ProblemData const * pd);

bool WinningGamestate(GameState const * gs, ProblemData const * pd);

void ComputePossibleContinuations(
    GameState const * gs,
    RoutingTable const * routing_table,
    ProblemData const * problem_data,
    ContinuationArray * continuations);

void ComputePlayerPossibleContinuations(
    player_t player_id,
    GameState const * gs,
    route_t obstructions,
    RoutingTable const * routing_table,
    ProblemData const * problem_data,
    ContinuationArray * continuations);

bool ValidateCohabitation(
    player_t player_id,
    location_t destination,
    GameState * ugs);

bool ValidatePath(
    GameState * ugs,
    RoutingTable const * routing_table,
    location_t source,
    location_t destination);

cost_t MovementCost(player_t player_id);

static inline player_t Colour(player_t player_id)
{
    return (player_t) (1 + (player_id % 4));
}

#endif
