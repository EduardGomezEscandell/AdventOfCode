#include "day2.h"
#include "common/file_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DEFINE_TEST(1, 150)
DEFINE_TEST(2, 900)

Command ReadCommand(char * line)
{
	Command command;
	for(char * it = line; *it != '\0'; ++it)
	{
		if(*it != ' ') continue;
		
		*it = '\0';
		++it;

		if(strcmp(line, "forward") == 0)   command.direction = FORWARD;
		else if(strcmp(line, "up") == 0)   command.direction = UP;
		else if(strcmp(line, "down") == 0) command.direction = DOWN;
		else
		{
			fprintf(stderr, "Unknown word: %s\n", line);
			exit(EXIT_FAILURE);
		}

		command.units = atoi(it);

		return command;
	}

	fprintf(stderr, "Line '%s' is not a vald command!\n", line);
	exit(EXIT_FAILURE);
}


int SolvePart1(bool is_test)
{
	FILE * file = GetFile(is_test, 2);
	
	char * line = NULL;
	size_t len = 0;
	ssize_t read = 0;

	int displ = 0;
	int depth = 0;
	
	while((read = getline(&line, &len, file)) != -1)
	{
		Command command = ReadCommand(line);

		switch (command.direction)
		{
			case FORWARD: displ += command.units; break;
			case UP: 	  depth -= command.units; break;
			case DOWN: 	  depth += command.units; break;
		}
	}

	free(line);
	fclose(file);

	return displ * depth;
}

int SolvePart2(bool is_test)
{
	FILE* file = GetFile(is_test, 2);

	char * line = NULL;
	size_t len = 0;
	ssize_t read = 0;

	int displ = 0;
	int depth = 0;
	int aim   = 0;
	
	while((read = getline(&line, &len, file)) != -1)
	{
		Command command = ReadCommand(line);

		switch (command.direction)
		{
		case FORWARD:
			displ += command.units;
			depth += command.units * aim;
			break;
		case UP:
			aim  -= command.units;
			break;
		case DOWN:
			aim  += command.units;
			break;
		}
	}

	free(line);
	fclose(file);

	return displ * depth;
}