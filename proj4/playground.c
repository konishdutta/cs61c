//
// Created by konis on 8/25/2024.
//

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
    int block_size = 64;  // This is an example block size. You should tune this based on your cache size.

    #pragma omp parallel for collapse(2)
    for (int bi = 0; bi < rows; bi += block_size) {
        for (int bj = 0; bj < cols; bj += block_size) {
            int i_end = (bi + block_size > rows) ? rows : bi + block_size;
            int j_end = (bj + block_size > cols) ? cols : bj + block_size;

            for (int i = bi; i < i_end; i += 4) {
                int j = bj;
                for (; j <= j_end - 4; j += 4) {
                    __m256d vec1 = _mm256_loadu_pd(&mat1->data[i][j]);
                    __m256d vec2 = _mm256_loadu_pd(&mat2->data[i][j]);
                    __m256d res1 = _mm256_add_pd(vec1, vec2);
                    _mm256_storeu_pd(&result->data[i][j], res1);

                    __m256d vec3 = _mm256_loadu_pd(&mat1->data[i + 1][j]);
                    __m256d vec4 = _mm256_loadu_pd(&mat2->data[i + 1][j]);
                    __m256d res2 = _mm256_add_pd(vec3, vec4);
                    _mm256_storeu_pd(&result->data[i + 1][j], res2);

                    __m256d vec5 = _mm256_loadu_pd(&mat1->data[i + 2][j]);
                    __m256d vec6 = _mm256_loadu_pd(&mat2->data[i + 2][j]);
                    __m256d res3 = _mm256_add_pd(vec5, vec6);
                    _mm256_storeu_pd(&result->data[i + 2][j], res3);

                    __m256d vec7 = _mm256_loadu_pd(&mat1->data[i + 3][j]);
                    __m256d vec8 = _mm256_loadu_pd(&mat2->data[i + 3][j]);
                    __m256d res4 = _mm256_add_pd(vec7, vec8);
                    _mm256_storeu_pd(&result->data[i + 3][j], res4);
                }
                // Handle remaining columns in the block
                for (; j < j_end; j++) {
                    result->data[i][j] = mat1->data[i][j] + mat2->data[i][j];
                    result->data[i + 1][j] = mat1->data[i + 1][j] + mat2->data[i + 1][j];
                    result->data[i + 2][j] = mat1->data[i + 2][j] + mat2->data[i + 2][j];
                    result->data[i + 3][j] = mat1->data[i + 3][j] + mat2->data[i + 3][j];
                }
            }

            // Handle remaining rows in the block
            for (int i = (i_end / 4) * 4; i < i_end; i++) {
                for (int j = bj; j < j_end; j++) {
                    result->data[i][j] = mat1->data[i][j] + mat2->data[i][j];
                }
            }
        }
    }

    return 0;
}
