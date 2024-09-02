#include "dumb_matrix.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <immintrin.h>
#include <x86intrin.h>
#endif

/* Below are some intel intrinsics that might be useful
 * void _mm256_storeu_pd (double * mem_addr, __m256d a)
 * __m256d _mm256_set1_pd (double a)
 * __m256d _mm256_set_pd (double e3, double e2, double e1, double e0)
 * __m256d _mm256_loadu_pd (double const * mem_addr)
 * __m256d _mm256_add_pd (__m256d a, __m256d b)
 * __m256d _mm256_sub_pd (__m256d a, __m256d b)
 * __m256d _mm256_fmadd_pd (__m256d a, __m256d b, __m256d c)
 * __m256d _mm256_mul_pd (__m256d a, __m256d b)
 * __m256d _mm256_cmp_pd (__m256d a, __m256d b, const int imm8)
 * __m256d _mm256_and_pd (__m256d a, __m256d b)
 * __m256d _mm256_max_pd (__m256d a, __m256d b)
*/

/*
 * Generates a random double between `low` and `high`.
 */
double rand_double(double low, double high) {
    double range = (high - low);
    double div = RAND_MAX / range;
    return low + (rand() / div);
}

/*
 * Generates a random matrix with `seed`.
 */
void rand_matrix(matrix *result, unsigned int seed, double low, double high) {
    srand(seed);
    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < result->cols; j++) {
            set(result, i, j, rand_double(low, high));
        }
    }
}

/*
 * Allocate space for a matrix struct pointed to by the double pointer mat with
 * `rows` rows and `cols` columns. You should also allocate memory for the data array
 * and initialize all entries to be zeros. Remember to set all fieds of the matrix struct.
 * `parent` should be set to NULL to indicate that this matrix is not a slice.
 * You should return -1 if either `rows` or `cols` or both have invalid values, or if any
 * call to allocate memory in this function fails. If you don't set python error messages here upon
 * failure, then remember to set it in numc.c.
 * Return 0 upon success and non-zero upon failure.
 */
int allocate_matrix(matrix **mat, int rows, int cols) {
    /* TODO: YOUR CODE HERE */
    // check for valid rows / cols
    if (rows < 1 || cols < 1) {
        return -1;
    }
    // alloc space for matrix
    *mat = malloc(sizeof(matrix));
    if (!*mat) {
        return -1;
    }
    // alloc memory for data array - initialize entries to be zero
    // data is an array of pointers to double pointers
    double **data = calloc(rows, sizeof(double *));
    if (!data) {
        free(*mat);
        return -1;
    }
    for (int i = 0; i < rows; i++) {
        // each item in data points to double pointers.
        double *dataCols = calloc(cols, sizeof(double));
        if (!dataCols) {
            for (int j = 0; j < i; j++) {
                free(data[j]);
            }
            free(data);
            free(*mat);
            return -1;
        }
        data[i] = dataCols;
    }
    // set the values for the matrix
    (*mat)->rows = rows;
    (*mat)->cols = cols;
    (*mat)->data = data;
    (*mat)->is_1d = (rows == 1 || cols == 1);
    (*mat)->ref_cnt = 1;
    (*mat)->parent = NULL;

    // return 0 for success    
    return 0;
}

/*
 * Allocate space for a matrix struct pointed to by `mat` with `rows` rows and `cols` columns.
 * This is equivalent to setting the new matrix to be
 * from[row_offset:row_offset + rows, col_offset:col_offset + cols]
 * If you don't set python error messages here upon failure, then remember to set it in numc.c.
 * Return 0 upon success and non-zero upon failure.
 */
int allocate_matrix_ref(matrix **mat, matrix *from, int row_offset, int col_offset,
                        int rows, int cols) {
    /* TODO: YOUR CODE HERE */
    // check if parent matrix has too few rows/cols
    if (rows < 1 || cols < 1 || row_offset < 0 || col_offset < 0) {
        return -1;
    }

    if (rows + row_offset > from->rows || cols + col_offset > from->cols) {
        return -1;
    }
    // alloc space for matrix
    *mat = malloc(sizeof(matrix));
    if (!*mat) {
        return -1;
    }
    double **data = calloc(rows, sizeof(double *));
    if (!data) {
        free(*mat);
        return -1;
    }
    // set the values for the matrix
    (*mat)->data = data;
    (*mat)->rows = rows;
    (*mat)->cols = cols;
    (*mat)->is_1d = (rows == 1 || cols == 1);
    (*mat)->ref_cnt = 1;
    (*mat)->parent = from;


    // adjust the data in the matrix
    for (int i = 0; i < rows; i++) {
        (*mat)->data[i] = from->data[i + row_offset] + col_offset;
    }

    // increment the parent ref counter
    from->ref_cnt += 1;

    return 0;
}

/*
 * This function will be called automatically by Python when a numc matrix loses all of its
 * reference pointers.
 * You need to make sure that you only free `mat->data` if no other existing matrices are also
 * referring this data array.
 * See the spec for more information.
 */
void deallocate_matrix(matrix *mat) {
    /* TODO: YOUR CODE HERE */
    if (!mat) return;
    mat->ref_cnt -= 1;
    if (mat->ref_cnt > 0) return;
    if (mat->parent) {
        mat->parent->ref_cnt -= 1;
        if (!mat->parent->ref_cnt) {
            deallocate_matrix(mat->parent);
        }
    } else {
        // free data
        for (int i = 0; i < mat->rows; i++) {
            free(mat->data[i]);
        }
        free(mat->data);
    }
    free(mat);
}

/*
 * Return the double value of the matrix at the given row and column.
 * You may assume `row` and `col` are valid.
 */
double get(matrix *mat, int row, int col) {
    /* TODO: YOUR CODE HERE */
    return mat->data[row][col];
}

/*
 * Set the value at the given row and column to val. You may assume `row` and
 * `col` are valid
 */
void set(matrix *mat, int row, int col, double val) {
    /* TODO: YOUR CODE HERE */
    mat->data[row][col] = val;
}

/*
 * Set all entries in mat to val
 */
void fill_matrix(matrix *mat, double val) {
    /* TODO: YOUR CODE HERE */
    for (int i = 0; i < mat->rows; i++) {
        for (int j = 0; j < mat->cols; j++) {
            mat->data[i][j] = val;
        }
    }
}

/*
 * Store the result of adding mat1 and mat2 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int add_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    /* TODO: YOUR CODE HERE */
    if (!result || !mat1 || !mat2) {
        return -1;
    }
    if (result->cols != mat1->cols || result->cols != mat2->cols) {
        return -1;
    }
    if (result->rows != mat1->rows || result->rows != mat2->rows) {
        return -1;
    }
    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < result->cols; j++) {

            //__m256d add_vec1 = _mm256_set_pd(mat1->data[i][j], mat1->data[i][j+1], mat1->data[i][j+2], mat1->data[i][j+3]);
            //__m256d add_vec2 = _mm256_set_pd(mat2->data[i][j], mat2->data[i][j+1], mat2->data[i][j+2], mat2->data[i][j+3]);
            //__m256d res_vec = _mm256_add_pd(add_vec1, add_vec2);
            //_mm256_store_pd(&(result->data[i][j]), res_vec);

            double d1 = mat1->data[i][j];
            double d2 = mat2->data[i][j];
            result->data[i][j] = d1 + d2;
        }
    }
    return 0;
}

/*
 * Store the result of subtracting mat2 from mat1 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int sub_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    /* TODO: YOUR CODE HERE */
    if (!result || !mat1 || !mat2) {
        return -1;
    }
    if (result->cols != mat1->cols || result->cols != mat2->cols) {
        return -1;
    }
    if (result->rows != mat1->rows || result->rows != mat2->rows) {
        return -1;
    }
    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < result->cols; j++) {
            double d1 = mat1->data[i][j];
            double d2 = mat2->data[i][j];
            result->data[i][j] = d1 - d2;
        }
    }
    return 0;    
}

/*
 * Store the result of multiplying mat1 and mat2 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 * Remember that matrix multiplication is not the same as multiplying individual elements.
 */
int mul_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    /* TODO: YOUR CODE HERE */
    if (!result || !mat1 || !mat2) {
        return -1;
    }
    if (result->rows != mat1->rows || result->cols != mat2->cols) {
        return -1;
    }
    if (mat1->cols != mat2->rows) {
        return -1;
    }
    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < result->cols; j++) {
            double val = 0;
            for (int k = 0; k < mat1->cols; k++) {
                val += (mat1->data[i][k]) * (mat2->data[k][j]);
            }
            result->data[i][j] = val;
        }
    }
    return 0;
}

void copy_matrix(matrix *from, matrix *to) {
    // assumes caller has already validated matrix dimensions
    for (int i = 0; i < from->rows; i++) {
        for (int j = 0; j < from->cols; j++) {
            set(to, i, j, get(from, i, j));
        }
    }
}

void create_identity_matrix(matrix *mat) {
    // assumes caller has checked mat is square
    fill_matrix(mat, 0);
    for (int i = 0; i < mat->rows; i++) {
        set(mat, i, i, 1);
    }
}

/*
 * Store the result of raising mat to the (pow)th power to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 * Remember that pow is defined with matrix multiplication, not element-wise multiplication.
 */

int pow_matrix(matrix *result, matrix *mat, int pow) {
    // check the matrices exist
    if (!result || !mat) {
        return -1;
    }
    // check they are square
    if (result->rows != mat->rows || result->cols != mat->cols) {
        return -1;
    }
    if (mat->rows != mat->cols) {
        return -1;
    }
    if (pow < 0) {
        return -1;
    }
    if (pow == 0) {
        create_identity_matrix(result);
        return 0;
    }
    if (pow == 1) {
        // if pow = 1, then just copy mat
        copy_matrix(mat, result);
        return 0;
    }

    // create a temp matrix and copy mat to it
    matrix **temp = malloc(sizeof(matrix *));
    if (!temp) {
        return -1;
    }
    if (allocate_matrix(temp, mat->rows, mat->cols) != 0) {
        free(temp);
        return -1;
    }

    copy_matrix(mat, *temp);

    if (pow % 2) {
        // odd case
        // x^3 = x(x^2)
        if (pow_matrix(*temp, mat, pow - 1) != 0) {
            deallocate_matrix(*temp);
            free(temp);
            return -1;
        }
        if (mul_matrix(result, mat, *temp) != 0) {
            deallocate_matrix(*temp);
            free(temp);
            return -1;
        }
    } else {
        // even case
        // x^4 = (x^2)^2
        if (mul_matrix(*temp, mat, mat) != 0) {
            deallocate_matrix(*temp);
            free(temp);
            return -1;
        }
        if (pow_matrix(result, *temp, pow / 2) != 0) {
            deallocate_matrix(*temp);
            free(temp);
            return -1;
        }
    }
    deallocate_matrix(*temp);
    free(temp);
    return 0;
}

/*
 * Store the result of element-wise negating mat's entries to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int neg_matrix(matrix *result, matrix *mat) {
    /* TODO: YOUR CODE HERE */
    if (!result || !mat) {
        return -1;
    }
    if (result->cols != mat->cols || result->rows != mat->rows) {
        return -1;
    }
    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < result->cols; j++) {
            double d = mat->data[i][j];
            result->data[i][j] = d * -1;
        }
    }
    return 0;
}

/*
 * Store the result of taking the absolute value element-wise to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int abs_matrix(matrix *result, matrix *mat) {
    /* TODO: YOUR CODE HERE */
    if (!result || !mat) {
        return -1;
    }
    if (result->cols != mat->cols || result->rows != mat->rows) {
        return -1;
    }
    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < result->cols; j++) {
            double d =  mat->data[i][j];
            if (d < 0) {
                result->data[i][j] = d * -1;
            } else {
                result->data[i][j] = d;
            }
        }
    }
    return 0;          
}


