#ifndef DAY_24_SIMULATE_H
#define DAY_24_SIMULATE_H

#include <stdlib.h>
#include <stdbool.h>

#include "common/hash_table.h"
#include "common/vector.h"

#define MONAD_DIGITS 14

typedef long int Int;

typedef Int (ChunckFun)(Int digit, Int z);


Int run_chunk_0(Int digit, Int z);
Int run_chunk_1(Int digit, Int z);
Int run_chunk_2(Int digit, Int z);
Int run_chunk_3(Int digit, Int z);
Int run_chunk_4(Int digit, Int z);
Int run_chunk_5(Int digit, Int z);
Int run_chunk_6(Int digit, Int z);
Int run_chunk_7(Int digit, Int z);
Int run_chunk_8(Int digit, Int z);
Int run_chunk_9(Int digit, Int z);
Int run_chunk_A(Int digit, Int z);
Int run_chunk_B(Int digit, Int z);
Int run_chunk_C(Int digit, Int z);
Int run_chunk_D(Int digit, Int z);

TEMPLATE_VECTOR(Int) IntArray;

TEMPLATE_HASH_TABLE(size_t, Int, IntArray, ContinuationsTable);

typedef struct 
{
	ChunckFun * eval;
	bool valid_continuations[10];
} ChunkData;

void FillChunkData(ChunkData data[14]);
Int Simulate(size_t depth, Int z, ChunkData chunk[MONAD_DIGITS]);

#endif
