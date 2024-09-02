#include "matrix.h"
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

int BLOCKSIZE = 32; // for blocking


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
    double **data = malloc(rows * sizeof(double *));
    if (!data) {
        free(*mat);
        return -1;
    }

	double *dataBlock = calloc(rows * cols, sizeof(double));
	if (!dataBlock) {
		free(data);
        free(*mat);
        return -1;
    }

	#pragma omp parallel for
    for (int i = 0; i < rows; i++) {
		data[i] = &(dataBlock[i * cols]);
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
    #pragma omp parallel for
    for (int i = 0; i < 4 * (rows / 4); i += 4) {
        data[i] = from->data[i + row_offset] + col_offset;
        data[i + 1] = from->data[i + row_offset + 1] + col_offset;
        data[i + 2] = from->data[i + row_offset + 2] + col_offset;
        data[i + 3] = from->data[i + row_offset + 3] + col_offset;
    }

    #pragma omp parallel for
    for (int i = 4 * (rows / 4); i < rows; i++) {
        data[i] = from->data[i + row_offset] + col_offset;
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
        if (mat->parent->ref_cnt <= 0) {
            deallocate_matrix(mat->parent);
        }
    } else {
        // free data
        free(mat->data[0]);
    }
    free(mat->data);
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
    __m256d vec_val = _mm256_set1_pd(val);

	#pragma omp parallel for
    for (int i = 0; i < mat->rows; i++) {
        for (int j = 0; j < 16*(mat->cols/16); j += 16) {
            _mm256_storeu_pd(&(mat->data[i][j]), vec_val);
            _mm256_storeu_pd(&(mat->data[i][j + 4]), vec_val);
            _mm256_storeu_pd(&(mat->data[i][j + 8]), vec_val);
            _mm256_storeu_pd(&(mat->data[i][j + 12]), vec_val);
        }

		for (int j = 16*(mat->cols/16); j < mat->cols; j++) {
			mat->data[i][j] = val;
		}
    }
}

/*
 * Store the result of adding mat1 and mat2 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int add_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    if (!result || !mat1 || !mat2) {
        return -1;
    }
    if (result->cols != mat1->cols || result->cols != mat2->cols) {
        return -1;
    }
    if (result->rows != mat1->rows || result->rows != mat2->rows) {
        return -1;
    }

    int rows = result->rows;
    int cols = result->cols;

	// block through 32 x 32 blocks assuming BLOCKSIZE is 32
	#pragma omp parallel for collapse(2)
	for (int bi = 0; bi < rows; bi += BLOCKSIZE) {
		for (int bj = 0; bj < cols; bj += BLOCKSIZE) {
			for (int i = bi; i < bi + BLOCKSIZE && i < rows; i++) {
				int j;
				for (j = bj; j < bj + BLOCKSIZE && j < 16*(cols/16); j+=16) {
					// load a bunch of vectors
            		__m256d vec1 = _mm256_loadu_pd(&mat1->data[i][j]);
           			__m256d vec2 = _mm256_loadu_pd(&mat2->data[i][j]);
            		__m256d res1 = _mm256_add_pd(vec1, vec2);
            		_mm256_storeu_pd(&result->data[i][j], res1);

					// unroll the loop
            		__m256d vec3 = _mm256_loadu_pd(&mat1->data[i][j + 4]);
           			__m256d vec4 = _mm256_loadu_pd(&mat2->data[i][j + 4]);
            		__m256d res2 = _mm256_add_pd(vec3, vec4);
            		_mm256_storeu_pd(&result->data[i][j + 4], res2);

            		__m256d vec5 = _mm256_loadu_pd(&mat1->data[i][j + 8]);
           			__m256d vec6 = _mm256_loadu_pd(&mat2->data[i][j + 8]);
            		__m256d res3 = _mm256_add_pd(vec5, vec6);
            		_mm256_storeu_pd(&result->data[i][j + 8], res3);

            		__m256d vec7 = _mm256_loadu_pd(&mat1->data[i][j + 12]);
           			__m256d vec8 = _mm256_loadu_pd(&mat2->data[i][j + 12]);
            		__m256d res4 = _mm256_add_pd(vec7, vec8);
            		_mm256_storeu_pd(&result->data[i][j + 12], res4);
				}
        		for (; j < bj + BLOCKSIZE && j < cols; j++) {
            		result->data[i][j] = mat1->data[i][j] + mat2->data[i][j];
        		}
			}
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

    int rows = result->rows;
    int cols = result->cols;

	// block through 32 x 32 blocks assuming BLOCKSIZE is 32
	#pragma omp parallel for collapse(2)
	for (int bi = 0; bi < rows; bi += BLOCKSIZE) {
		for (int bj = 0; bj < cols; bj += BLOCKSIZE) {
			for (int i = bi; i < bi + BLOCKSIZE && i < rows; i++) {
				int j;
				for (j = bj; j < bj + BLOCKSIZE && j < 16*(cols/16); j+=16) {
					// load a bunch of vectors
            		__m256d vec1 = _mm256_loadu_pd(&mat1->data[i][j]);
           			__m256d vec2 = _mm256_loadu_pd(&mat2->data[i][j]);
            		__m256d res1 = _mm256_sub_pd(vec1, vec2);
            		_mm256_storeu_pd(&result->data[i][j], res1);

					// unroll the loop
            		__m256d vec3 = _mm256_loadu_pd(&mat1->data[i][j + 4]);
           			__m256d vec4 = _mm256_loadu_pd(&mat2->data[i][j + 4]);
            		__m256d res2 = _mm256_sub_pd(vec3, vec4);
            		_mm256_storeu_pd(&result->data[i][j + 4], res2);

            		__m256d vec5 = _mm256_loadu_pd(&mat1->data[i][j + 8]);
           			__m256d vec6 = _mm256_loadu_pd(&mat2->data[i][j + 8]);
            		__m256d res3 = _mm256_sub_pd(vec5, vec6);
            		_mm256_storeu_pd(&result->data[i][j + 8], res3);

            		__m256d vec7 = _mm256_loadu_pd(&mat1->data[i][j + 12]);
           			__m256d vec8 = _mm256_loadu_pd(&mat2->data[i][j + 12]);
            		__m256d res4 = _mm256_sub_pd(vec7, vec8);
            		_mm256_storeu_pd(&result->data[i][j + 12], res4);
				}
        		for (; j < bj + BLOCKSIZE && j < cols; j++) {
            		result->data[i][j] = mat1->data[i][j] - mat2->data[i][j];
        		}
			}
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
    #pragma omp parallel for collapse(2)
    for (int bi = 0; bi < mat1->rows; bi += BLOCKSIZE) {
        for (int bj = 0; bj < mat2->cols; bj += BLOCKSIZE) {
            for (int bk = 0; bk < mat1->cols; bk += BLOCKSIZE) {

                for (int i = bi; i < bi + BLOCKSIZE && i < mat1->rows; i++) {
                    for (int j = bj; j < bj + BLOCKSIZE && j < mat2->cols; j++) {
                        // create a sum vector that we'll use to capture intermediate values for res[i][j]
                        __m256d sum_vec = _mm256_setzero_pd();
                        for (int k = bk; k < bk + BLOCKSIZE && k < 16 * (mat1->cols / 16); k += 16) {
                            // load a bunch of vectors
                            __m256d vec1 = _mm256_loadu_pd(&mat1->data[i][k]);
                            __m256d vec2 = _mm256_set_pd(
                                mat2->data[k + 3][j],
                                mat2->data[k + 2][j],
                                mat2->data[k + 1][j],
                                mat2->data[k][j]
                            );
                            sum_vec = _mm256_add_pd(sum_vec, _mm256_mul_pd(vec1, vec2));

                            // unroll the loop
                            __m256d vec3 = _mm256_loadu_pd(&mat1->data[i][k + 4]);
                            __m256d vec4 = _mm256_set_pd(
                                mat2->data[k + 7][j],
                                mat2->data[k + 6][j],
                                mat2->data[k + 5][j],
                                mat2->data[k + 4][j]
                            );
                            sum_vec = _mm256_add_pd(sum_vec, _mm256_mul_pd(vec3, vec4));

                            __m256d vec5 = _mm256_loadu_pd(&mat1->data[i][k + 8]);
                            __m256d vec6 = _mm256_set_pd(
                                mat2->data[k + 11][j],
                                mat2->data[k + 10][j],
                                mat2->data[k + 9][j],
                                mat2->data[k + 8][j]
                            );
                            sum_vec = _mm256_add_pd(sum_vec, _mm256_mul_pd(vec5, vec6));

                            __m256d vec7 = _mm256_loadu_pd(&mat1->data[i][k + 12]);
                            __m256d vec8 = _mm256_set_pd(
                                mat2->data[k + 15][j],
                                mat2->data[k + 14][j],
                                mat2->data[k + 13][j],
                                mat2->data[k + 12][j]
                            );
                            sum_vec = _mm256_add_pd(sum_vec, _mm256_mul_pd(vec7, vec8));
                        }
                        double temp[4];
                        _mm256_storeu_pd(temp, sum_vec);
                        result->data[i][j] += (temp[0] + temp[1] + temp[2] + temp[3]);

                        for (int k = 16 * (mat1->cols / 16); k < bk + BLOCKSIZE && k < mat1->cols; k++) {
                            result->data[i][j] += ((mat1->data[i][k]) * (mat2->data[k][j]));
                        }
                    }
                }
            }
        }
    }
    return 0;
}

void copy_matrix(matrix *from, matrix *to) {
    // assumes caller has already validated matrix dimensions
    for (int i = 0; i < from->rows; i++) {
        #pragma omp parallel for
        for (int j = 0; j < from->cols; j++) {
            set(to, i, j, get(from, i, j));
        }
    }
}

void create_identity_matrix(matrix *mat) {
    // assumes caller has checked mat is square
    fill_matrix(mat, 0);
    #pragma omp parallel for
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
    // create a mask for negation
    uint64_t mask = ~0;
    mask = ~(mask >> 1);
    __m256d mask_vec = _mm256_castsi256_pd(_mm256_set1_epi64x(mask));

    #pragma omp parallel for
    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < 16 * (result->cols / 16); j += 16) {
            __m256d vec1 = _mm256_loadu_pd(&mat->data[i][j]);
            __m256d vec2 = _mm256_loadu_pd(&mat->data[i][j + 4]);
            __m256d vec3 = _mm256_loadu_pd(&mat->data[i][j + 8]);
            __m256d vec4 = _mm256_loadu_pd(&mat->data[i][j + 12]);

            __m256d res1 = _mm256_xor_pd(vec1, mask_vec);
            __m256d res2 = _mm256_xor_pd(vec2, mask_vec);
            __m256d res3 = _mm256_xor_pd(vec3, mask_vec);
            __m256d res4 = _mm256_xor_pd(vec4, mask_vec);

            _mm256_storeu_pd(&result->data[i][j], res1);
            _mm256_storeu_pd(&result->data[i][j + 4], res2);
            _mm256_storeu_pd(&result->data[i][j + 8], res3);
            _mm256_storeu_pd(&result->data[i][j + 12], res4);

        }
        for (int j = 16 * (result->cols/16); j < result->cols; j++) {
            union {
                double d;
                uint64_t u;
            } mask_val;
            mask_val.d = mat->data[i][j];
            mask_val.u ^= mask;
            result->data[i][j] = mask_val.d;
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
	// create a mask for absolute value
	uint64_t mask = ~0;
	mask = (mask >> 1);
	__m256d mask_vec = _mm256_castsi256_pd(_mm256_set1_epi64x(mask));
    #pragma omp parallel for
    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < 16 * (result->cols / 16); j += 16) {
            __m256d vec1 = _mm256_loadu_pd(&mat->data[i][j]);
            __m256d vec2 = _mm256_loadu_pd(&mat->data[i][j + 4]);
            __m256d vec3 = _mm256_loadu_pd(&mat->data[i][j + 8]);
            __m256d vec4 = _mm256_loadu_pd(&mat->data[i][j + 12]);

            __m256d res1 = _mm256_and_pd(vec1, mask_vec);
            __m256d res2 = _mm256_and_pd(vec2, mask_vec);
            __m256d res3 = _mm256_and_pd(vec3, mask_vec);
            __m256d res4 = _mm256_and_pd(vec4, mask_vec);

            _mm256_storeu_pd(&result->data[i][j], res1);
            _mm256_storeu_pd(&result->data[i][j + 4], res2);
            _mm256_storeu_pd(&result->data[i][j + 8], res3);
            _mm256_storeu_pd(&result->data[i][j + 12], res4);

        }
        for (int j = 16 * (result->cols/16); j < result->cols; j++) {
            union {
                double d;
                uint64_t u;
            } mask_val;
            mask_val.d = mat->data[i][j];
            mask_val.u &= mask;
            result->data[i][j] = mask_val.d;
        }
    }
    return 0;          
}


