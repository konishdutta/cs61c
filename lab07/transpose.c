#include "transpose.h"

/* The naive transpose function as a reference. */
void transpose_naive(int n, int blocksize, int *dst, int *src) {
    for (int x = 0; x < n; x++) {
        for (int y = 0; y < n; y++) {
            dst[y + x * n] = src[x + y * n];
        }
    }
}

/* Implement cache blocking below. You should NOT assume that n is a
 * multiple of the block size. */
void transpose_blocking(int n, int blocksize, int *dst, int *src) {
    for (int x = 0; x < n; x+=blocksize) {
        for (int y = 0; y < n; y+=blocksize) {
            int blockWidth = blocksize;
            if (x + blocksize > n) {
                blockWidth = n - x;
            }
            int blockHeight = blocksize;
            if (y + blocksize > n) {
                blockHeight = n - y;
            }
            // height * width
            for (int w = 0; w < blockWidth; w++) {
                for (int h = 0; h < blockHeight; h++) {
                    dst[(h + y) + ((w + x) * n)] = src[(w + x) + ((h + y)* n)];
                }
            }
        }
    }
}

/*
Why does it require matrix to be a certain size?
blocksize = 20
n = number
the matrix has to be larger than 20 x 20 which is 400


[0] [1] [2] [3] [4] [5] [6]
[0] [1] [2] [3] [4] [5] [6]
[0] [1] [2] [3] [4] [5] [6]
[0] [1] [2] [3] [4] [5] [6]
[0] [1] [2] [3] [4] [5] [6]
[0] [1] [2] [3] [4] [5] [6]
[0] [1] [2] [3] [4] [5] [6]

dst[(h + y) + ((w + x) * n)] = src[(w + x) + ((h + y)* n)];

n = 7
blocksize = 2
h = 0, w = 0, x = 0, y = 0; dst[0] src[0]
h = 1, w = 0, x = 0, y = 0; dst[1] src[7]
h = 0, w = 1, x = 0, y = 0; dst[7] src[1]
h = 1, w = 1, x = 0, y = 0; dst[8] src[8]
h = 0, w = 0, x = 2, y = 0; dst[14] src[2]
h = 1, w = 0, x = 2, y = 0; dst[15] src[9]
h = 0, w = 1, x = 2, y = 0; dst[21] src[3]
h = 1, w = 0, x = 2, y = 0; dst[22] src[10]
...
h = 0, w = 0, x = 2, y = 2; dst[16] src[16]
h = 1, w = 0, x = 2, y = 2; dst[17] src[23]
h = 0, w = 1, x = 2, y = 2; dst[23] src[17]
h = 1, w = 1, x = 2, y = 2; dst[24] src[24]

h = 0, w = 0, x = 5, y = 5; dst[40] src[40]
h = 1, w = 0, x = 5, y = 5; dst[41] src[47]
h = 0, w = 1, x = 5, y = 5; dst[47] src[41]
h = 1, w = 1, x = 5, y = 5; dst[48] src[48]


h = 0, w = 0, x = 5, y = 6; dst[41] src[47]
h = 1, w = 0, x = 5, y = 6; dst[42] src[49] -- no op
h = 0, w = 1, x = 5, y = 6; dst[48] src[48]
h = 1, w = 1, x = 5, y = 6; dst[49] src[42] -- no op

*/