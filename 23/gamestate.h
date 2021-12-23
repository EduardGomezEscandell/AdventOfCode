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
 */
typedef uint_least64_t gamestate_t;
typedef uint_least8_t player_t;

TEMPLATE_VECTOR(gamestate_t) GamestateArray;

#define NPLAYERS 8

typedef struct
{
    location_t locations[NPLAYERS];
    bool moveflags[NPLAYERS];
    bool blockades[NLOCS];
} UnpackedGamestate;

UnpackedGamestate UnpackGamestate(gamestate_t gs);
gamestate_t PackGamestate(UnpackedGamestate * ugs);
UnpackedGamestate CopyLocationsAndFlags(UnpackedGamestate const * source);

void GetLine(char ** line, FILE * file);
gamestate_t ReadGamestate(FILE * file);

location_t GetLocation(gamestate_t gs, player_t player);


void ComputePossibleContinuations(
    gamestate_t gs,
    RoutingTable const * routing_table,
    GamestateArray * continuation,
    CostArray * costs);

#endif

