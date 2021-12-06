#include "day6.h"
#include "common/file_utils.h"

School ParseSchool(FILE * file)
{
	char * line = NULL;
	size_t len = 0;
	ssize_t read = 0;

	if((read = getline(&line, &len, file)) == -1)
	{
		fprintf(stderr, "Failed to read file\n");
		exit(EXIT_FAILURE);
	}

	School school;
	for(size_t i=0; i<CHILDHOOD+1; ++i)
	{
		school.fish[i] = 0;
	}

	char * begin = line;
	for(char * end = line; *end != '\0'; ++end)
	{
		if(*end == '\n' || *end == ',')
		{
			*end = '\0';
			size_t reverse_age = atoi(begin);
			++school.fish[reverse_age];
			
			begin = end + 1;
		}
	}
	free(line);

	return school;
}

void NextDay(School * school)
{
	unsigned long new_fish = school->fish[0];
	for(size_t i=0; i<CHILDHOOD; ++i)
	{
		school->fish[i] = school->fish[i+1];
	}
	
	school->fish[CHILDHOOD] = new_fish;
	school->fish[REPR_CYCLE] += new_fish;
}

unsigned long CountFish(const School * const school)
{
	unsigned long count = 0;
	for(size_t i=0; i<=CHILDHOOD; ++i)
	{
		count += school->fish[i];
	}
	return count;
}

void PrintSchool(const School * const s)
{
	printf("{ ");
	for(size_t i = 0; i < CHILDHOOD+1; ++i)
	{
		printf(" %d:%ld ", (int) i, s->fish[i]);
	}
	printf(" }");
}


unsigned long RunSimulation(const bool is_test, const size_t n_days)
{
	FILE* file = GetFile(is_test, 6);

	School school = ParseSchool(file); // School of fish
	fclose(file);

	for(size_t day = 0; day != n_days; ++day)
	{
		NextDay(&school);
	}

	const unsigned long n_fish = CountFish(&school);

	return n_fish;
}

unsigned long SolvePart1(const bool is_test)
{
	const size_t n_days = 80;
	return RunSimulation(is_test, n_days);
}

unsigned long SolvePart2(const bool is_test)
{
	const size_t n_days = 256;
	return RunSimulation(is_test, n_days);
}
