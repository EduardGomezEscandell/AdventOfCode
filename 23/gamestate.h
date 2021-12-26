#ifndef DAY_23_GAMESTATE_H
#define DAY_23_GAMESTATE_H

#include "common/vector.h"
#include "routing.h"

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>


typedef int_least32_t player_t;

#define MAX_PLAYERS 16
#define NO_PLAYER 0XFF

typedef struct problem_data_ {
    player_t n_players;
    location_t n_locations;
    int part;
} ProblemData;

typedef struct
{
    location_t locations[MAX_PLAYERS];
    bool moveflags[MAX_PLAYERS];
    route_t blockades;
} GameState;

typedef struct 
{
    cost_t cost;
    GameState state;
} Continuation;

TEMPLATE_VECTOR(Continuation) ContinuationArray;

void GetLine(char ** line, FILE * file);
GameState ReadGamestate(FILE * file, ProblemData const * pdata);

location_t GetLocation(GameState const * gs, player_t player);

void PrettyPrintGamestate(GameState const * gs, ProblemData const * pd);

bool WiningGamestate(GameState const * gs, ProblemData const * pd);

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

static inline player_t CorrectRoom(player_t player_id)
{
    return (player_t) (1 + (player_id % 4));
}

#endif
