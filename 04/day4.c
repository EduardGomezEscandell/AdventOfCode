#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "day4.h"
#include "common/file_utils.h"


BoardVector NewBoardVector()
{
	BoardVector v;
	v.capacity = 10;
	v.begin = malloc(v.capacity * sizeof(Board));
	v.end = v.begin;
	return v;
}

size_t BVLength(const BoardVector v)
{
	return v.end - v.begin;
}

Board * BVExpand(BoardVector * v)
{
	const size_t length = BVLength(*v);
	if(length < v->capacity)
	{
		++v->end;
		return v->end - 1;
	}
	
	v->capacity *= 1.8;
	v->begin = realloc(v->begin, v->capacity * sizeof(Board));
	v->end = v->begin + length + 1;

	return v->end - 1;
}

void FillRow(Cell row[9], char * line)
{
	char * prev_start = line;
	size_t i = 0;

	for(char * it = line; *it != '\0'; ++it)
	{
		while(*prev_start == ' ')
		{
			++prev_start;
			++it;
		}

		if(*it == ' ' || *it == '\n')
		{
			*it = '\0';
			row[i].value = atoi(prev_start);
			row[i].marked = false;
			++i;

			if(i == 5) return;

			prev_start = it + 1;
		}
	}
}


BoardVector ReadBoards(FILE* file)
{
	BoardVector v = NewBoardVector();

	char * line = NULL;
	size_t len = 0;
	ssize_t read = 0;


	read = getline(&line, &len, file);
	while(read != -1)
	{
		Board * b = BVExpand(&v);
		b->finished = false;

		while(line[0] == '\n')
		{
			read = getline(&line, &len, file);
			if(read == -1) exit(EXIT_FAILURE);
		}

		for(unsigned int i=0; i<5; ++i)
		{
			FillRow(b->data[i], line);
			read = getline(&line, &len, file);
		}
	}

	free(line);
	return v;
}

Cell * UpdateBoard(Board * b, const int n)
{
	bool victory = false;
	Cell * winner_cell = NULL;
	for(size_t i=0; i<5; ++i)
	{
		bool fullrow = true;
		for(size_t j=0; j<5; ++j)
		{
			if(b->data[i][j].value == n)
			{
				b->data[i][j].marked = true;
				winner_cell = &(b->data[i][j]);
			}
			fullrow = fullrow && b->data[i][j].marked;
		}
		victory = victory || fullrow;
	}

	for(size_t i=0; i<5; ++i)
	{
		bool fullcol = true;
		for(size_t j=0; j<5; ++j)
		{
			fullcol = fullcol && b->data[j][i].marked;
		}
		victory = victory || fullcol;
	}

	if(victory) b->finished = true;

	return victory ? winner_cell : NULL;
}

int CountPoints(const Board b, int winner_number)
{
	int points = 0;
	for(size_t i=0; i<5; ++i)
	{
		for(size_t j=0; j<5; ++j)
		{
			if(b.data[i][j].marked) continue;
			points += b.data[i][j].value;
		}
	}
	return points * winner_number;
}


int SolvePart2(const bool is_test)
{
	FILE* file = GetFile(is_test, 4);

	char * line = NULL;
	size_t len = 0;
	ssize_t read = 0;

	if((read = getline(&line, &len, file)) == -1) exit(EXIT_FAILURE);
	char * calls = malloc((strlen(line)+1) * sizeof(char));
	strcpy(calls, line);
	free(line);

	BoardVector boards = ReadBoards(file);
	fclose(file);

	char * prev_start = calls;
	Board * winner = NULL;
	int winner_number = 0;

	for(char * c = calls; *c != '\0'; ++c)
	{
		if(*c == ',' || *c == '\n')
		{
			*c = '\0';
			int call = atoi(prev_start);
			prev_start = c+1;

			size_t n_boards = BVLength(boards);
			for(Board * it=boards.begin; it != boards.end; ++it)
			{
				if(it->finished){
					--n_boards;
					continue;
				} 
				UpdateBoard(it, call);
				if(it->finished)
				{
					winner = it;
					--n_boards;
				}
			}

			if(n_boards == 0)
			{
				winner_number = call;
				break;
			}
		}
	}

	if(winner == NULL)
	{
		printf("oh no!\n");
		exit(EXIT_FAILURE);
	}

	int score = CountPoints(*winner, winner_number);
	free(boards.begin);
	free(calls);
	return score;
}

int SolvePart1(const bool is_test)
{
	FILE* file = GetFile(is_test, 4);

	char * line = NULL;
	size_t len = 0;
	ssize_t read = 0;

	if((read = getline(&line, &len, file)) == -1) exit(EXIT_FAILURE);
	char * calls = malloc((strlen(line)+1) * sizeof(char));
	strcpy(calls, line);
	free(line);

	BoardVector boards = ReadBoards(file);
	fclose(file);

	char * prev_start = calls;
	Board * winner = NULL;
	Cell * winner_cell = NULL;
	int winner_number = 0;
	for(char * c = calls; *c != '\0'; ++c)
	{
		if(*c == ',' || *c == '\n')
		{
			*c = '\0';
			int call = atoi(prev_start);
			prev_start = c+1;

			winner_cell = NULL;
			for(Board * it=boards.begin; it != boards.end; ++it)
			{
				winner_cell = UpdateBoard(it, call);
				if(winner_cell)
				{
					winner = it;
					winner_number = call;
					break;
				}
			}
			if(winner_cell) break;
		}
	}

	if(winner == NULL) exit(EXIT_FAILURE);

	int score = CountPoints(*winner, winner_number);
	free(boards.begin);
	free(calls);
	return score;
}
