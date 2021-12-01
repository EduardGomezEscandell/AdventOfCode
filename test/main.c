#include <stdio.h>
#include "common/mystring.h"
#include "common/vector.h"

int main()
{
	String hello = CreateString("Hello");
	String world = CreateString("world");
	
	StringAppendC(&hello, " ");
	StringAppend(&hello, world);
	StringClear(world);

	StringPrintLine(hello);
	StringClear(hello);

	Vector v = CreateVector(2, 0);
	VectorPop(&v);
	VectorPush(&v, 1);
	VectorPush(&v, 2);
	VectorPush(&v, 3);
	VectorPrintLine(v);
	VectorClear(v);
}
