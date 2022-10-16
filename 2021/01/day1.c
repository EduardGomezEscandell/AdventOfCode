#include "day1.h"
#include "common/file_utils.h"

DEFINE_TEST(1, 7)
DEFINE_TEST(2, 5)

int SolvePart1(bool is_test)
{

	FILE* file = GetFile(is_test, 1);

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

int SolvePart2(bool is_test)
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
	
	int windows[3] = {0, 0, 0};

	size_t i = 0;
	int old_result = -1;
	int increases = 0;

	while((read = getline(&line, &len, file) != -1))
	{
		int data = atoi(line);
		++i;

		// Adding data
		windows[0] += data;
		windows[1] += data;
		windows[2] += data;

		// Finishing windows
		size_t filled_window = i % 3;

		int latest_result = windows[filled_window];
		windows[filled_window] = 0;

		if(i > 3) // not first window
		{
			increases += latest_result > old_result ? 1 : 0;
		}
		old_result = latest_result;

	}

	fclose(file);
	free(line);

	return increases;
}