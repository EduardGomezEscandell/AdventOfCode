#ifndef DAY_23_GAMESTATE_H
#define DAY_23_GAMESTATE_H

#include "common/vector.h"
#include "routing.h"

#include <stdint.h>
#include <stdio.h>

/* Gamestate:
 *  FEDC BA98 7654 3210 FEDC BA98 7654 3210 FEDC BA98 7654 3210 FEDC BA98 76543210
 *  0000 0000 0000 0000 0000 0000 AAAA aaaa BBBB bbbb CCCC cccc DDDD dddd AaBb CcDd
 *  ^ignored bits            ^Locations (4 bits each)                      ^~~~~~~~~ Has moved flag (1 bit each)
 */
typedef uint_least64_t gamestate_t;
typedef uint_least8_t player_t;

TEMPLATE_VECTOR(gamestate_t) GamestateArray;


void GetLine(char ** line, FILE * file);
gamestate_t ReadGamestate(FILE * file);

location_t GetLocation(gamestate_t gs, player_t player);

#endif

