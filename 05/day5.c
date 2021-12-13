#include "day5.h"
#include "common/file_utils.h"
#include "common/math.h"

DEFINE_TEST(1, 5)
DEFINE_TEST(2, 12)

Board EmptyBoard()
{
	Board b;
	for(size_t x=0; x<BOARD_SIZE_X; ++x)
	{
		for(size_t y=0; y<BOARD_SIZE_Y; ++y)
		{
			b.data[x][y] = 0;
		}
	}
	return b;
}

Vent LineToVent(char * const line)
{
	Vent v;

	// Skip until ','
	char * begin=line;
	char * end = NULL;
	for(end = begin; *end != ','; ++end) { }
	*end = '\0';
	
	v.x1 = (size_t) atoi(begin);

	// Skip until ' '
	begin = end+1;
	for(end = begin; *end != ' '; ++end) { }
	*end = '\0';
	
	v.y1 = (size_t) atoi(begin);
	
	// Skip from " ->  " until ','
	begin = end+4; // " -> "
	for(end = begin; *end != ','; ++end) { }
	*end = '\0';
	
	v.x2 = (size_t) atoi(begin);

	// Skip until ' '
	begin = end+1;
	for(end = begin; *end != '\n'; ++end) { }
	*end = '\0';

	v.y2 = (size_t) atoi(begin);

	return v;
}

void AddVentToBoard(
	Board * const board,
	const Vent * const v,
	const bool consider_diagonals)
{
	const bool is_horizontal = (v->y1 == v->y2);
	if(is_horizontal)
	{
		size_t start = MIN(v->x1, v->x2);
		size_t end = MAX(v->x1, v->x2);
		size_t y = v->y1;

		for(size_t x=start; x <= end; ++x)
		{
			++(board->data[x][y]);
		}
		return;
	}

	const bool is_vertical = (v->x1 == v->x2);
	if(is_vertical)
	{
		size_t start = MIN(v->y1, v->y2);
		size_t end = MAX(v->y1, v->y2);
		size_t x = v->x1;

		for(size_t y=start; y <= end; ++y)
		{
			++(board->data[x][y]);
		}
		return;
	}

	if(!consider_diagonals) return;

	const int dx = (v->x1 < v->x2) ? 1 : -1;
	const int dy = (v->y1 < v->y2) ? 1 : -1;

	size_t x = v->x1;
	size_t y = v->y1;

	const size_t n = dx > 0 ? (v->x2 - v->x1) : (v->x1 - v->x2);
	for(size_t i=0; i<=n; ++i)
	{
		++(board->data[x][y]);
		x += dx;
		y += dy;
	}
}


void ReadVents(FILE * file, Board * board, const bool consider_diagonals)
{
	char * line = NULL;
	size_t len = 0;
	ssize_t read = 0;
	
	while((read = getline(&line, &len, file)) != -1)
	{
		Vent v = LineToVent(line);
		// printf("%d,%d -> %d,%d\n", v.x1, v.y1, v.x2, v.y2);
		AddVentToBoard(board, &v, consider_diagonals);
	}

	free(line);
}

void PrintBoard(const Board * const b)
{
	for(size_t y=0; y<BOARD_SIZE_Y; ++y)
	{
		for(size_t x=0; x<BOARD_SIZE_X; ++x)
		{
			if(b->data[x][y] == 0) printf(".");
			else                   printf("%d", b->data[x][y]);
		}
		printf("\n");
	}
	printf("\n");
}

unsigned int CountOverlaps(const Board * const b)
{
	unsigned int count =0;
	for(size_t y=0; y<BOARD_SIZE_Y; ++y)
	{
		for(size_t x=0; x<BOARD_SIZE_X; ++x)
		{
			if(b->data[x][y] > 1) ++count;
		}
	}
	return count;
}

int SolvePart1(const bool is_test)
{
	FILE* file = GetFile(is_test, 5);

	Board b = EmptyBoard();
	ReadVents(file, &b, false);
	fclose(file);

	// PrintBoard(&b);

	return CountOverlaps(&b);
}

int SolvePart2(const bool is_test)
{
	FILE* file = GetFile(is_test, 5);

	Board b = EmptyBoard();
	ReadVents(file, &b, true);
	fclose(file);

	// PrintBoard(&b);

	return CountOverlaps(&b);
}
