#include "file_utils.h"
#include <stdlib.h>

FILE* GetFile(const bool is_test, const unsigned int day)
{

	const char * filename = is_test ? "test" :  "data";
	
	char full_filename[17];
	sprintf(full_filename, "data/%02d/%s.txt", day, filename);

	FILE* file = fopen(full_filename, "r");

	if (file == NULL)
	{
		fprintf(stderr, "File not found: %s", full_filename);
		exit(EXIT_FAILURE);
	}

	return file;
}