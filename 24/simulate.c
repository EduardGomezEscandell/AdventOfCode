#include "simulate.h"
#include "common/hash_table.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

ChunckFun* GetFunction(size_t depth)
{
	switch (depth) {
		case 0x0: return &run_chunk_0;
		case 0x1: return &run_chunk_1;
		case 0x2: return &run_chunk_2;
		case 0x3: return &run_chunk_3;
		case 0x4: return &run_chunk_4;
		case 0x5: return &run_chunk_5;
		case 0x6: return &run_chunk_6;
		case 0x7: return &run_chunk_7;
		case 0x8: return &run_chunk_8;
		case 0x9: return &run_chunk_9;
		case 0xA: return &run_chunk_A;
		case 0xB: return &run_chunk_B;
		case 0xC: return &run_chunk_C;
		case 0xD: return &run_chunk_D;
	}

	return NULL;
}

void FillChunkData(ChunkData data[14])
{
	for(size_t i=0; i<MONAD_DIGITS; ++i)
	{
		data[i].eval = GetFunction(i);
	}
}


Int run_chunk_0(Int digit, Int z)
{
    return digit + 8 + z;
}

Int run_chunk_1(Int digit, Int z)
{
    return 26*z + digit + 16;
}

Int run_chunk_2(Int digit, Int z)
{
    return 26*z + digit + 4;
}

Int run_chunk_3(Int digit, Int z)
{
    bool c3 = (z%26 - 11) != digit;
    return z/26 * (25*c3 + 1) + (digit + 1)*c3;
}

Int run_chunk_4(Int digit, Int z)
{
    return 26*z + digit + 13;
}

Int run_chunk_5(Int digit, Int z)
{
    return 26*z + digit + 5;
}

Int run_chunk_6(Int digit, Int z)
{
    return  26*z + digit;
}

Int run_chunk_7(Int digit, Int z)
{
    bool c7 = (z%26 - 5) != digit;
    return (z/26)*(25*c7+1) + (digit+10)*c7;
}

Int run_chunk_8(Int digit, Int z)
{
    return 26*z + digit + 7;
}

Int run_chunk_9(Int digit, Int z)
{
    bool c9 = (z%26) != digit;
    return (z/26) * (25*c9 + 1) + (digit + 2)*c9;
}

Int run_chunk_A(Int digit, Int z)
{
    bool cA = (z%26 - 11) != digit;
    return (z/26)*(25*cA + 1) + (digit+13)*cA;
}

Int run_chunk_B(Int digit, Int z)
{
    bool cB = (z%26 - 13) != digit;
    return (z/26)*(25*cB + 1) + (digit + 15)*cB;
}

Int run_chunk_C(Int digit, Int z)
{
    bool cC = (z%26 - 13) != digit;
    return (z/26)*(25*cC + 1) + (digit + 14)*cC;
}

Int run_chunk_D(Int digit, Int z)
{
    bool cD = ((z%26) - 11) != digit;
    z = (z/26) * (25*cD + 1) + (digit + 9)*cD;

    return z == 0 ? 0 : 1; // Reducing number of outputs
}
