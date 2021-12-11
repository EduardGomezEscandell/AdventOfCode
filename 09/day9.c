#include <string.h>

#include "day9.h"
#include "common/file_utils.h"

int * StringToInts(char * line, const size_t len)
{
	int * ints = malloc(len * sizeof(int));

	for(size_t i=0; i < len; ++i)
	{
		ints[i] = line[i] - '0';
	}

	return ints;
}


HeightMap ReadHeightMap(FILE * file)
{
	char * line = NULL;
	size_t len = 0;
	ssize_t read = 0;

	HeightMap hmap;
	size_t capacity = 10;
	hmap.z = malloc(capacity * sizeof(int*));
	hmap.x_span = 0;
	hmap.y_span = 0;
	
	size_t y = 0;
	while((read = getline(&line, &len, file)) != -1)
	{
		if(hmap.x_span == 0)
		{
			hmap.x_span = strlen(line) - 1;
		}

		if(y >= capacity)
		{
			capacity *= 1.6;
			hmap.z = realloc(hmap.z, capacity * sizeof(int*));
		}

		hmap.z[y] = StringToInts(line, hmap.x_span);

		++hmap.y_span;
		++y;
	}

	hmap.basin = malloc(hmap.y_span * sizeof(size_t*));
	for(size_t y=0; y<hmap.y_span; ++y)
	{
		hmap.basin[y] = calloc(hmap.x_span, sizeof(size_t));
	}

	free(line);
	fclose(file);

	return hmap;
}


void ClearHeightMap(HeightMap * hmap)
{
	for(size_t y=0; y<hmap->y_span; ++y)
	{
		free(hmap->z[y]);
		hmap->z[y] = NULL;

		free(hmap->basin[y]);
		hmap->basin[y] = NULL;
	}
	free(hmap->z);
	free(hmap->basin);
	hmap->z = NULL;
	hmap->basin = NULL;
	hmap->x_span = 0;
	hmap->y_span = 0;
}

void PrintHeightMap(HeightMap * hmap)
{
	for(size_t y=0; y<hmap->y_span; ++y)
	{
		for(size_t x=0; x<hmap->x_span; ++x)
			printf("%d", hmap->z[y][x]);
		printf("\n");
	}
}

void PrintBasins(HeightMap * hmap)
{
	for(size_t y=0; y<hmap->y_span; ++y)
	{
		for(size_t x=0; x<hmap->x_span; ++x)
			printf("%ld", hmap->basin[y][x]);
		printf("\n");
	}
}

int GetDangerScore(HeightMap * hmap, const size_t x, const size_t y)
{
	if(x != 0              && hmap->z[y][x] >= hmap->z[y][x-1]) return 0;
	if(x != hmap->x_span-1 && hmap->z[y][x] >= hmap->z[y][x+1]) return 0;
	if(y != 0              && hmap->z[y][x] >= hmap->z[y-1][x]) return 0;
	if(y != hmap->y_span-1 && hmap->z[y][x] >= hmap->z[y+1][x]) return 0;
	
	return hmap->z[y][x] + 1;
}


size_t ExploreDownstream(HeightMap * hmap, size_t x, size_t y)
{
	// printf("[%ld, %ld](%d)\n", x, y, hmap->z[y][x]);

	if(hmap->z[y][x] == 9)
	{
		hmap->basin[y][x] = 0;
		return 0;	
	} 
	if(hmap->basin[y][x]) return hmap->basin[y][x];

	if(x != 0              && hmap->z[y][x] > hmap->z[y][x-1])
	{
		hmap->basin[y][x] = ExploreDownstream(hmap, x-1, y);
		return hmap->basin[y][x];
	}

	if(x != hmap->x_span-1 && hmap->z[y][x] > hmap->z[y][x+1])
	{
		hmap->basin[y][x] = ExploreDownstream(hmap, x+1, y);
		return hmap->basin[y][x];
	}

	if(y != 0              && hmap->z[y][x] > hmap->z[y-1][x])
	{
		hmap->basin[y][x] = ExploreDownstream(hmap, x, y-1);
		return hmap->basin[y][x];
	}

	if(y != hmap->y_span-1 && hmap->z[y][x] > hmap->z[y+1][x])
	{
		hmap->basin[y][x] = ExploreDownstream(hmap, x, y+1);
		return hmap->basin[y][x];
	}
	
	return 0;
}


void LabelBasins(HeightMap * hmap)
{
	for(size_t y=0; y<hmap->y_span; ++y)
	{
		for(size_t x=0; x<hmap->x_span; ++x)
		{
			if(hmap->z[y][x] == 9) continue;
			if(hmap->basin[y][x] > 0) continue;

			ExploreDownstream(hmap, x, y);
		}
	}
}

void Insert(Basin * list, size_t n_sorted, size_t i)
{
	// [9, 7, 6, 2, 4, 2, 3, 1, 8, 5, 2]
	//           ^n_sorted=3    ^i
	// Result:
	// [9, 8, 7, 2, 4, 2, 3, 1, 6, 5, 2]
	//                          ^i

	for(size_t pos=0; pos < n_sorted; ++pos)
	{
		if(list[i].size > list[pos].size) // Find position where to insert
		{
			Basin tmp = list[i];

			// Push everything back
			list[i] = list[n_sorted-1];

			for(size_t j = n_sorted-1; j > pos; --j)
			{
				list[j] = list[j-1];
			}

			// Insert
			list[pos] = tmp;
		}
	}
}


void FindLargestBasins(Basin largest_basins[3], const HeightMap * const hmap, const size_t n_basins)
{
	Basin * basins = malloc(n_basins * sizeof(Basin));
	for(size_t i=0; i<n_basins; ++i)
	{
		basins[i].id = i;
		basins[i].size = 0;
	}

	for(size_t y=0; y<hmap->y_span; ++y)
	{
		for(size_t x=0; x<hmap->x_span; ++x)
		{
			const size_t b = hmap->basin[y][x];
			++basins[b].size;
		}
	}

	basins[0].size = 0; // Ensuring invalid-id basin does not win

	// Sorting top-three positions with insert-sort
	size_t n_sorted = 1;
	for(size_t i=1; i<n_basins; ++i)
	{
		Insert(basins, n_sorted, i);
		n_sorted += n_sorted<3 ? 1 : 0;
	}

	// for(size_t i=0; i<n_basins; ++i)
	// 	printf("%ld: %ld\n", basins[i].id, basins[i].size);

	for(size_t i=0; i<3; ++i)
	{
		largest_basins[i] = basins[i];
	}

	free(basins);
}

int SolvePart1(const bool is_test)
{
	FILE * file = GetFile(is_test, 9);

	HeightMap hmap = ReadHeightMap(file);

	int total_danger = 0;
	for(size_t y=0; y<hmap.y_span; ++y)
	{
		for(size_t x=0; x<hmap.x_span; ++x)
		{
			total_danger += GetDangerScore(&hmap, x, y);
		}
	}

	ClearHeightMap(&hmap);

	return total_danger;
}

int SolvePart2(const bool is_test)
{
	FILE * file = GetFile(is_test, 9);

	HeightMap hmap = ReadHeightMap(file);

	// Locating minima
	int basin_id = 1;
	for(size_t y=0; y<hmap.y_span; ++y)
	{
		for(size_t x=0; x<hmap.x_span; ++x)
		{
			if(GetDangerScore(&hmap, x, y))
			{
				hmap.basin[y][x] = basin_id;
				++basin_id;
			}
		}
	}

	LabelBasins(&hmap);
	// PrintBasins(&hmap);
	
	Basin basins[3];
	FindLargestBasins(basins, &hmap, basin_id);

	size_t result = 1;
	for(size_t i=0; i<3; ++i)
	{
		result *= basins[i].size;
	}

	ClearHeightMap(&hmap);

	return result;
}