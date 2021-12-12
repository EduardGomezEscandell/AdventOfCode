#include "day12.h"
#include "common/file_utils.h"

size_t HashString(const char * const begin, const char * const end)
{
	const size_t base = 'z' - 'a';
	size_t hash = 0;
	for(char const * it = begin; it != end; ++it)
	{
		size_t digit = *it >= 'a' ? *it - 'a' : *it - 'A';
		hash = hash*base + digit;
	}
	return hash;
}


Cave NewCave(const size_t id, CaveType type)
{
	Cave c;
	c.id = id;
	c.n_visits = 0;
	c.type = type;
	c.neighbours = NULL;
	c.n_neighbours = 0;
	return c;
}

void SetAsNeighbours(Cave * cave1, Cave * cave2)
{
	++cave1->n_neighbours;
	cave1->neighbours = realloc(cave1->neighbours, cave1->n_neighbours * sizeof(Cave*));
	cave1->neighbours[cave1->n_neighbours-1] = cave2;

	++cave2->n_neighbours;
	cave2->neighbours = realloc(cave2->neighbours, cave2->n_neighbours * sizeof(Cave*));
	cave2->neighbours[cave2->n_neighbours-1] = cave1;
}

void ClearCave(Cave * cave)
{
	free(cave->neighbours);
	cave->neighbours = NULL;
	cave->n_neighbours = 0;
}

size_t StartId()
{
	const char * str = "start";
	char const * begin = &str[0];
	char const * end = &str[5];
	return HashString(begin, end);
}

size_t EndId()
{
	const char * str = "end";
	char const * begin = &str[0];
	char const * end = &str[3];
	return HashString(begin, end);
}

CaveSystem NewCaveSystem()
{
	CaveSystem cs;
	cs.capacity = 100;
	cs.begin = malloc(cs.capacity * sizeof(Cave));
	cs.end = cs.begin+2;

	cs.begin[0] = NewCave(StartId(), START);
	cs.begin[1] = NewCave(EndId(), END);

	return cs;
}

void ClearCaveSystem(CaveSystem * cs)
{
	for(Cave *c = cs->begin; c != cs->end; ++c)
		ClearCave(c);

	free(cs->begin);
	cs->begin = NULL;
	cs->end = NULL;
	cs->capacity = 0;
}

Cave * FindCave(CaveSystem * cs, const size_t id_lookup, const CaveType type)
{
	for(Cave * cave = cs->begin; cave != cs->end; ++cave)
	{
		if(cave->id == id_lookup && cave->type == type) return cave;
	}
	return NULL;
}

CaveType Classify(const char * const begin, const size_t hash)
{
	if(hash == StartId()) return START;
	if(hash == EndId()) return END;

	if(*begin >= 'a') return SMALL;

	return BIG;
}

void ReadCavesInLine(size_t ids[2], CaveType types[2], char * line)
{
	size_t i = 0;
	char * begin = line;
	for(char * end = line; *end != '\0'; ++end)
	{
		if(*end == '-' || *end == '\n')
		{
			ids[i] = HashString(begin, end);
			types[i] = Classify(begin, ids[i]);
			++i;
			begin = end + 1;
		}
	}
}

void PrintCave(const Cave * const cave)
{
	printf("Cave id: %ld, neighbours: ", cave->id);

	for(size_t i=0; i<cave->n_neighbours; ++i)
	{
		printf("%ld ", cave->neighbours[i]->id);
	}
}


void PrintCaveSystem(const CaveSystem * const cs)
{
	printf("CaveSystem: {\n");
	for(Cave * cave = cs->begin; cave != cs->end; ++cave)
	{
		printf("    ");
		PrintCave(cave);
		printf("\n");
	}
	printf("}\n");
}

Cave * PushCave(CaveSystem * cs, const size_t id, const CaveType type)
{
	const size_t n_caves = cs->end - cs->begin;
	if(n_caves >= cs->capacity)
	{
		fprintf(stderr, "Not enough memory in cave system!");
		exit(EXIT_FAILURE);
		// Using a realloc would invalidate all neighbour pointers
	}

	*cs->end = NewCave(id, type);
	++cs->end;

	return cs->end - 1;
}

CaveSystem ReadCaves(FILE * file)
{
	char * line = NULL;
	size_t len = 0;
	ssize_t read = 0;

	CaveSystem cs = NewCaveSystem();

	size_t connectivity [2];
	CaveType caves_types[2];
	while((read = getline(&line, &len, file)) != -1)
	{		
		ReadCavesInLine(connectivity, caves_types, line);

		Cave * c1 = FindCave(&cs, connectivity[0], caves_types[0]);
		Cave * c2 = FindCave(&cs, connectivity[1], caves_types[1]);

		if(!c1) c1 = PushCave(&cs, connectivity[0], caves_types[0]);
		if(!c2) c2 = PushCave(&cs, connectivity[1], caves_types[1]);

		SetAsNeighbours(c1, c2);

	}

	free(line);
	fclose(file);

	return cs;
}

size_t CountPaths(CaveSystem *cs, Cave * start, unsigned int max_small_visits)
{
	if(start->type == END)   return 1;
	if(start->type == START && start->n_visits >= 1) return 0;
	if(start->type == SMALL && start->n_visits >= max_small_visits) return 0;

	size_t n_paths = 0;

	if(start->type == SMALL && start->n_visits > 0) --max_small_visits;

	++start->n_visits;

	for(size_t i=0; i<start->n_neighbours; ++i)
	{
		n_paths += CountPaths(cs, start->neighbours[i], max_small_visits);
	}

	--start->n_visits;
	return n_paths;
}

int Solve(const bool is_test, const unsigned int max_small_visits)
{
	FILE * file = GetFile(is_test, 12);

	CaveSystem cs = ReadCaves(file);
	size_t n_paths = CountPaths(&cs, cs.begin, max_small_visits);

	ClearCaveSystem(&cs);
	return n_paths;
}

int SolvePart1(const bool is_test)
{
	return Solve(is_test, 1);
}

int SolvePart2(const bool is_test)
{
	return Solve(is_test, 2);
}