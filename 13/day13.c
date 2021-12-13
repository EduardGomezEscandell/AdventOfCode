#include "day13.h"

#include <string.h>
#include "common/file_utils.h"

FoldVector EmptyFoldVector()
{
	FoldVector fv;
	fv.capacity = 10;
	fv.begin = malloc(fv.capacity * sizeof(Fold));
	fv.end = fv.begin;
	return fv;
}

void FoldVectorPush(FoldVector * fv, char axis, size_t value)
{
	const size_t size = fv->end - fv->begin;
	
	if(size >= fv->capacity)
	{
		fv->capacity *= 1.6;
		fv->begin = realloc(fv->begin, fv->capacity * sizeof(Fold));
		fv->end = fv->begin + size;
	}

	fv->end->axis = axis;
	fv->end->value = value;

	++fv->end;
}

void FoldVectorClear(FoldVector * fv)
{
	free(fv->begin);
	fv->begin = NULL;
	fv->end = NULL;
	fv->capacity = 0;
}

void ReadLine(SparseMatrix * sp, char * line)
{
	char * begin = line;
	size_t i=0;
	long data[2] = {-1, -1};

	for(char * it = line; *it != '\0'; ++it)
	{
		if(*it == ',' || *it == '\n')
		{
			*it = '\0';
			data[i++] = atol(begin);
			begin = it+1;
		}
	}
	SpTriplet t;
	t.row = data[1];
	t.col = data[0];
	t.data = 1;
	
	SpPush(sp, &t);
}

void ReadData(FILE * file, SparseMatrix * sp, FoldVector * folds)
{
	char * line = NULL;
	size_t len = 0;
	ssize_t read = 0;

	*sp = NewSparseMatrix();
	*folds = EmptyFoldVector();

	while((read = getline(&line, &len, file)) != -1)
	{		
		if(*line == '\n') break;

		ReadLine(sp, line);
	}

	while((read = getline(&line, &len, file)) != -1)
	{
		FoldVectorPush(folds, line[11], atoi(&line[13]));
	}

	free(line);
	fclose(file);
}

void YFold(SparseMatrix * sp, const size_t y_fold)
{
	for(SpTriplet * it = sp->begin; it != sp->end; ++it)
	{
		if(it->row < y_fold) continue;

		if(it->row == y_fold) {
			it->data = 0;
			continue;
		}

		it->row = 2*y_fold - it->row;
	}

	sp->nrows = y_fold;
	SpPopZeros(sp);
	SpMergeDuplicates(sp);
}

void XFold(SparseMatrix * sp, const size_t x_fold)
{
	for(SpTriplet * it = sp->begin; it != sp->end; ++it)
	{
		if(it->col < x_fold) continue;

		if(it->col == x_fold) {
			it->data = 0;
			continue;
		}

		it->col = 2*x_fold - it->col;
	}

	sp->ncols = x_fold;
	SpPopZeros(sp);
	SpMergeDuplicates(sp);
}


int SolvePart1(const bool is_test)
{
	FILE * file = GetFile(is_test, 13);

	SparseMatrix sp;
	FoldVector folds;

	ReadData(file, &sp, &folds);	
	
	switch(folds.begin->axis)
	{
		case 'x': XFold(&sp, folds.begin->value); break;
		case 'y': YFold(&sp, folds.begin->value); break;
		default:
			fprintf(stderr, "Wronng axis: %c!\n", folds.begin->axis);
			exit(EXIT_FAILURE);
	}

	const size_t n_entries = sp.end - sp.begin;

	ClearSparseMatrix(&sp);
	FoldVectorClear(&folds);

	return n_entries;
}

int SolvePart2(const bool is_test)
{
	FILE * file = GetFile(is_test, 13);

	SparseMatrix sp;
	FoldVector folds;

	ReadData(file, &sp, &folds);	
	
	for(Fold * it=folds.begin; it != folds.end; ++it)
	{
		switch(it->axis)
		{
			case 'x': XFold(&sp, it->value); break;
			case 'y': YFold(&sp, it->value); break;
			default:
				fprintf(stderr, "Wronng axis: %c!\n", folds.begin->axis);
				exit(EXIT_FAILURE);
		}
	}

	if(is_test) printf("Test 2 solution:\n");
	else        printf("Part 2 solution:\n");
	SpPrintSparsity(&sp);

	// Yeah... not going to implement an intepreter for this.
	// Solution for my dataset looks like:
	// 
	// ###  #### #  # #### #    ###   ##  #  # 
	// #  #    # # #     # #    #  # #  # #  # 
	// #  #   #  ##     #  #    #  # #    #### 
	// ###   #   # #   #   #    ###  # ## #  # 
	// # #  #    # #  #    #    #    #  # #  # 
	// #  # #### #  # #### #### #     ### #  # 
	// 

	ClearSparseMatrix(&sp);
	FoldVectorClear(&folds);

	return is_test;
}