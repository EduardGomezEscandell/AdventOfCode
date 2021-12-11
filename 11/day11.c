#include <string.h>

#include "day11.h"
#include "common/file_utils.h"

void StringToRow(long * result, char * line, const size_t len)
{
	for(size_t col=0; col<len; ++col)
	{
		result[col] = line[col] - '0';
	}
}

Matrix EmptyMatrix(size_t rows, size_t cols)
{
	Matrix mat;

	size_t min_capacity = 10;
	
	mat.crows = rows > min_capacity ? rows : min_capacity;
	mat.ccols = cols > min_capacity ? cols : min_capacity;
	
	mat.nrows = rows;
	mat.ncols = cols;
	
	mat.data = malloc(mat.crows * sizeof(long*));

	for(size_t row=0; row < mat.nrows; ++row)
	{
		mat.data[row] = calloc(mat.ccols, sizeof(long));
	}

	return mat;
}

void AddRow(Matrix * mat)
{
	if(mat->nrows >= mat->crows)
	{
		mat->crows *= 1.6;
		mat->data = realloc(mat->data, mat->crows * sizeof(long*));
	}

	mat->data[mat->nrows] = calloc(mat->ccols, sizeof(long));

	++mat->nrows;
}

void AddCol(Matrix * mat)
{
	if(mat->ncols >= mat->ccols)
	{
		mat->ccols *= 1.6;
		for(size_t row=0; row < mat->nrows; ++mat) {
			mat->data[row] = realloc(mat->data[row], mat->ccols * sizeof(long));
			mat->data[row][mat->ncols] = 0;
		}
	}
	else
	{
		for(size_t row=0; row < mat->nrows; ++mat)
			mat->data[row][mat->ncols] = 0;
	}
	++mat->ncols;
}


Matrix ReadMatrix(FILE * file)
{
	char * line = NULL;
	size_t len = 0;
	ssize_t read = 0;

	Matrix mat;
	
	size_t row = 0;
	while((read = getline(&line, &len, file)) != -1)
	{
		if(row == 0) {
			mat = EmptyMatrix(0, strlen(line) - 1);
		}

		AddRow(&mat);
		StringToRow(mat.data[row], line, mat.ncols);

		++row;
	}

	// PrintMatrix(&mat);

	free(line);
	fclose(file);

	return mat;
}


void ClearMatrix(Matrix * mat)
{
	for(size_t row=0; row<mat->nrows; ++row)
	{
		free(mat->data[row]);
		mat->data[row] = NULL;
	}
	free(mat->data);
	mat->data = NULL;
	mat->ncols = 0;
	mat->nrows = 0;
	mat->ccols = 0;
	mat->crows = 0;
}

void PrintMatrix(const Matrix * const mat)
{
	for(size_t y=0; y<mat->nrows; ++y)
	{
		for(size_t x=0; x<mat->ncols; ++x)
			if( mat->data[y][x] > 9) printf("%c", (char) ('a' + mat->data[y][x] - 10));
			else if( mat->data[y][x] < 0) printf("%c", (char) ('A' - mat->data[y][x] - 1));
			else                     printf("%ld", mat->data[y][x]);
		printf("\n");
	}
	printf("\n");
}

bool Flash(Matrix * mat, const size_t row, const size_t col)
{
	if(mat->data[row][col] < 10) return false;

	const size_t i0 = row==0            ? row : row-1;
	const size_t i1 = row+1==mat->nrows ? row : row+1;

	const size_t j0 = col==0            ? col : col-1;
	const size_t j1 = col+1==mat->ncols ? col : col+1;

	mat->data[row][col] = -1;

	for(size_t i=i0; i<=i1; ++i)
	{
		for(size_t j=j0; j<=j1; ++j)
		{
			if(mat->data[i][j] > 0)
				++mat->data[i][j];
		}
	}

	return true;
}

unsigned int NextStep(Matrix * mat)
{
	// Step 1: Increase
	for(size_t i=0; i<mat->nrows; ++i) {
		for(size_t j=0; j<mat->ncols; ++j) {
			++mat->data[i][j];
		}
	}

	// Step 2: Flashing
	bool any_flashes = true;

	while(any_flashes) {
		any_flashes = false;
		for(size_t i=0; i<mat->nrows; ++i) {
			for(size_t j=0; j<mat->ncols; ++j) {
				any_flashes = any_flashes || Flash(mat, i, j);
			}
		}
	}

	// Step 3: Reseting and counting
	unsigned int n_flashes = 0;
	for(size_t i=0; i<mat->nrows; ++i) {
		for(size_t j=0; j<mat->ncols; ++j) {
			if(mat->data[i][j] > 0) continue;
			++n_flashes;
			mat->data[i][j] = 0;
		}
	}

	return n_flashes;
}

int SolvePart1(const bool is_test)
{
	FILE * file = GetFile(is_test, 11);

	Matrix mat = ReadMatrix(file);

	const size_t n_steps = 100;
	unsigned int n_flashes = 0;

	for(size_t step=0; step < n_steps; ++step)
	{
		n_flashes += NextStep(&mat);
	}


	ClearMatrix(&mat);
	return n_flashes;
}

int SolvePart2(const bool is_test)
{
	FILE * file = GetFile(is_test, 11);

	Matrix mat = ReadMatrix(file);

	const size_t n_entries = mat.nrows * mat.ncols;

	size_t n_steps = 0;
	
	unsigned int n_flashes = 0;
	while(n_flashes != n_entries)
	{
		n_flashes = NextStep(&mat);
		++n_steps;

		// printf("After step %ld:\n", n_steps);
		// PrintMatrix(&mat);
	}

	ClearMatrix(&mat);
	return n_steps;
}