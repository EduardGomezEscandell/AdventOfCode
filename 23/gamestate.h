#ifndef DAY_23_GAMESTATE_H
#define DAY_23_GAMESTATE_H

#include "common/vector.h"

#include <stdint.h>
#include <stdio.h>

/* Gamestate:
 *
 *  000000000000000000000000 AAAAaaaaBBBBbbbbCCCCccccDDDDdddd AaBbCcDd
 *  ^ignored bits            ^Locations (4 bits each)        ^ Has moved flag (1 bit each)
 */
typedef uint_least64_t gamestate_t;


TEMPLATE_VECTOR(gamestate_t) GamestateArray;


void GetLine(char ** line, FILE * file);
gamestate_t ReadGamestate(FILE * file);

int GetLocation(gamestate_t gs, size_t player);

#endif

