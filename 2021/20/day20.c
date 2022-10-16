#include "day20.h"
#include "common/file_utils.h"
#include "common/matrix.h"

#include <stddef.h>

void BinaryAppend(binary_t * binary, bool bit)
{
    *binary = (*binary << 1) + bit;
}

BoolArray ReadAlgorithm(FILE * file)
{
    size_t len = 0;
    char * line = 0;

    if(getline(&line, &len, file) == -1)
    {
        free(line);
        fprintf(stderr, "Wrong file! (%s:%d)\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    BoolArray algo;
    NEW_VECTOR(algo);
    RESERVE(algo, 512);

    for(const char * it = line; *it != '\n' && *it != '\0'; ++it)
    {
        bool value = *it=='#';
        PUSH(algo, value);
    }

    // Skipping blank line
    if(getline(&line, &len, file) == -1)
    {
        free(line);
        fprintf(stderr, "Wrong file! (%s:%d)\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    free(line);
    return algo;
}

SparseMatrix ReadImage(FILE * file)
{
    size_t len = 0;
    char * line = 0;

    SparseMatrix image = NewSparseMatrix();

    size_t row = 0;
    while(getline(&line, &len, file) != -1)
    {
        size_t col = 0;
        for(const char * it = line; *it != '\n' && *it != '\0'; ++it)
        {
            spdata_type value = *it=='#';
            SpPush(&image, row, col, &value);
            ++col;
        }
        ++row;
    }

    free(line);
    return image;
}

binary_t ExtractNeighbourhood(SparseMatrix * image, ssize_t row, ssize_t col, bool fill_value)
{
    binary_t bin = 0;
    for(ssize_t r = row-1; r != row+2; ++r) {
        for(ssize_t c = col-1; c != col+2; ++c)
        {
            if(r < image->n_negcols
               || r >= image->nrows
               || c < image->n_negrows
               || c >= image->ncols)
                BinaryAppend(&bin, fill_value);
            else{
                bool datum = SpRead(image, r, c) > 0;
                BinaryAppend(&bin, datum);
            }
        }
    }

    return bin;
}

void Update(SparseMatrix * image, BoolArray algorithm, bool * fill_value)
{
    SparseMatrix out = NewSparseMatrix();
    SpReserve(&out, SpNumNonZero(image));

    for(ssize_t row=image->n_negrows-5; row<image->nrows+5; ++row) {
        for(ssize_t col=image->n_negcols-5; col<image->ncols+5; ++col)
        {
            binary_t neighbourhood = ExtractNeighbourhood(image, row, col, *fill_value);
            spdata_type value = algorithm.begin[neighbourhood];
            SpPush(&out, row, col, &value);
        }
    }

    binary_t distant_neighbourhood = *fill_value ? 0x1FF : 0; // 0x1FF is bin(111111111)
    *fill_value = algorithm.begin[distant_neighbourhood];

    ClearSparseMatrix(image);
    *image = out;
}

solution_t CountNonZero(SparseMatrix * image)
{
    solution_t count = 0;
    for(ssize_t row=image->n_negrows-1; row<image->nrows+1; ++row) {
        for(ssize_t col=image->n_negcols-1; col<image->ncols+1; ++col)
        {
            count += SpRead(image, row, col) > 0;
        }
    }
    return count;
}

solution_t Solve(const bool is_test, const size_t n_iterations)
{
    // if(!is_test) return 0;

    FILE * file = GetFile(is_test, 20);

    BoolArray algorithm = ReadAlgorithm(file);
    SparseMatrix image = ReadImage(file);
    fclose(file);

    bool fill_value = 0;

    for(size_t i=0; i<n_iterations; ++i){
        Update(&image, algorithm, &fill_value);
    }

    solution_t nnz = SpNumNonZero(&image);

    CLEAR(algorithm);
    ClearSparseMatrix(&image);

    return nnz;
}

solution_t SolvePart1(const bool is_test)
{
    return Solve(is_test, 2);
}


solution_t SolvePart2(const bool is_test)
{
    return Solve(is_test, 50);
}


DEFINE_TEST(1, 35)
DEFINE_TEST(2, 3351)
