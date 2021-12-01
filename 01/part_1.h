#ifndef DAY1_PART_1_H
#define DAY1_PART_1_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int SolvePart1(bool is_test)
{
	char* filename;
	if(is_test)
	{
		filename = "data/01/test.txt";
	} else {
		filename = "data/01/data.txt";
	}
	 

	FILE* file = fopen(filename, "r");

	if (file == NULL) exit(EXIT_FAILURE);

	char * line = NULL;
	size_t len = 0;
	size_t read = 0;

	int increases = 0;
	
	read = getline(&line, &len, file);
	
	int old_data = atoi(line);

	while((read = getline(&line, &len, file) != -1))
	{
		int data = atoi(line);
		increases += (data > old_data) ? 1 : 0;
		old_data = data;
	}

	fclose(file);
	free(line);

	return increases;
}

#endif