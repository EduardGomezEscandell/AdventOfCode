#include "day11.h"
#include "common/file_utils.h"

DEFINE_TEST(1, 1656)
DEFINE_TEST(2, 195)

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