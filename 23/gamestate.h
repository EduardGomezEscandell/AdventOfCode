#ifndef DAY_23_GAMESTATE_H
#define DAY_23_GAMESTATE_H

#include "common/vector.h"
#include "routing.h"

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>


/* Gamestate:
 *  64 bit number encoding info:
 *
 *  0000 0000 0000 0000 0000 0000 AAAA aaaa BBBB bbbb CCCC cccc DDDD dddd AaBb CcDd
 *  ^~~~~~~~ignored bits~~~~~~~~^ ^~~~player locations (4 bits each)~~~~^ ^~~~~~~~^ Has moved flag (1 bit each)
 *                              
 */
typedef uint_least64_t gamestate_t;
typedef int_least16_t player_t;

TEMPLATE_VECTOR(gamestate_t) GamestateArray;

#define NPLAYERS 8
#define NO_PLAYER 8

typedef struct
{
    location_t locations[NPLAYERS];
    bool moveflags[NPLAYERS];
    route_t blockades;
} UnpackedGamestate;

UnpackedGamestate UnpackGamestate(gamestate_t gs);
gamestate_t PackGamestate(UnpackedGamestate * ugs);
UnpackedGamestate CopyLocationsAndFlags(UnpackedGamestate const * source);

typedef struct 
{
    cost_t cost;
    gamestate_t state;
} Continuation;

TEMPLATE_VECTOR(Continuation) ContinuationArray;

void GetLine(char ** line, FILE * file);
gamestate_t ReadGamestate(FILE * file);

location_t GetLocation(gamestate_t gs, player_t player);

void PrettyPrintGamestate(UnpackedGamestate * ugs);

bool WiningGamestate(gamestate_t gs);

void ComputePossibleContinuations(
    gamestate_t gs,
    RoutingTable const * routing_table,
    ContinuationArray * continuations);

void ComputePlayerPossibleContinuations(
    player_t player_id,
    UnpackedGamestate * ugs,
    RoutingTable const * routing_table,
    ContinuationArray * continuations);

bool ValidateCohabitation(
    player_t player_id,
    location_t destination,
    UnpackedGamestate * ugs);

bool ValidatePath(
    UnpackedGamestate * ugs,
    RoutingTable const * routing_table,
    location_t source,
    location_t destination);

int MovementCost(short player_id);

static inline player_t CorrectRoom(player_t player_id)
{
    return (player_t) (1 + (player_id / 2));
}

#endif
