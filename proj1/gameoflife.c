#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "imageloader.h"
void getNeighbors(Image *, uint32_t, uint32_t, uint32_t []);
uint32_t isAlive(uint8_t);
//Determines what color the cell at the given row/col should be. This function allocates space for a new Color.
//Note that you will need to read the eight neighbors of the cell in question. The grid "wraps", so we treat the top row as adjacent to the bottom row
//and the left column as adjacent to the right column.
Color *evaluateOneCell(Image *image, int row, int col, uint32_t rule)
{
	//YOUR CODE HERE
    Color *currColor = &(image->image[row][col]);

    uint8_t colors[3] = {currColor->R, currColor->G, currColor->B};
    uint32_t neighbors[3] = {0, 0, 0};
    getNeighbors(image, row, col, neighbors); //modify neighbors

    for (int i = 0; i < 3; i++) {
        uint32_t alive = isAlive(colors[i]);
        //first 9 bits in the rule used for dead cells. second 9 bits used for live cells
        uint32_t position = alive ? (neighbors[i] + 9) : neighbors[i];
        //use the mask to perform a bitwise op to determine whether next gen lives
        uint32_t mask = 1 << position;
        uint32_t finalCall = rule & mask;
        if (!alive && finalCall) {
            colors[i] = 255;
        } else if (alive && !finalCall) {
            colors[i] = 0;
        }
    }

    Color *newColor = (Color *) malloc(sizeof(Color));
    if (newColor == NULL) {
        return NULL;
    }

    newColor->R = colors[0];
    newColor->G = colors[1];
    newColor->B = colors[2];

    return newColor;
}

void getNeighbors(Image *image, uint32_t row, uint32_t col, uint32_t res[]) {
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) {
                continue;
            }
            uint32_t currRow = (row + i + image->rows) % image->rows;
            uint32_t currCol = (col + j + image->cols) % image->cols;
            Color evalColor = image->image[currRow][currCol];
            if (isAlive(evalColor.R)) {
                res[0]++;
            }
            if (isAlive(evalColor.G)) {
                res[1]++;
            }
            if (isAlive(evalColor.B)) {
                res[2]++;
            }
        }
    }
}

uint32_t isAlive(uint8_t color) {
    return color != 0;
}

//The main body of Life; given an image and a rule, computes one iteration of the Game of Life.
//You should be able to copy most of this from steganography.c
Image *life(Image *image, uint32_t rule)
{
	//YOUR CODE HERE
    Image *res = malloc(sizeof(Image));
    if (res == NULL) {
        return NULL;
    }

    res->rows = image->rows;
    res->cols = image->cols;

    Color **new_colors = (Color **) malloc(sizeof(Color *) * image->rows);
    if (new_colors == NULL) {
        return NULL;
    }

    for (int i = 0; i < image->rows; i++) {
        new_colors[i] = (Color *) malloc(sizeof(Color) * image->cols);
        if (new_colors[i] == NULL) {
            for (int k = 0; k < i; k++) {
                free(new_colors[k]);
            }
            free(new_colors);
            free(res);
            return NULL;
        }

        for (int j = 0; j < image->cols; j++) {
            Color *color_pointer = evaluateOneCell(image, i, j, rule);
            if (color_pointer == NULL) {
                for (int k = 0; k < i; k++) {
                    free(new_colors[k]);
                }
                free(new_colors);
                free(res);
                return NULL;
            }
            new_colors[i][j] = *(color_pointer);
            free(color_pointer);
        }
    }
    res->image = new_colors;
    return res;
}

/*
Loads a .ppm from a file, computes the next iteration of the game of life, then prints to stdout the new image.

argc stores the number of arguments.
argv stores a list of arguments. Here is the expected input:
argv[0] will store the name of the program (this happens automatically).
argv[1] should contain a filename, containing a .ppm.
argv[2] should contain a hexadecimal number (such as 0x1808). Note that this will be a string.
You may find the function strtol useful for this conversion.
If the input is not correct, a malloc fails, or any other error occurs, you should exit with code -1.
Otherwise, you should return from main with code 0.
Make sure to free all memory before returning!

You may find it useful to copy the code from steganography.c, to start.
*/
int main(int argc, char **argv)
{
	//YOUR CODE HERE
    if (argc != 3) {
        printf("usage: ./gameOfLife filename rule");
        printf("\nfilename is an ASCII PPM (type P3) with maximum value 255");
        printf("\nrule is a hex number beginning with 0x; Life is 0x1808");
        return 1;
    }
    Image *curr = readData(argv[1]);
    if (curr == NULL) {
        return -1;
    }
    Image *next = life(curr, (uint32_t)strtoul(argv[2], NULL, 0));

    if (next == NULL) {
        freeImage(curr);
        return -1;
    }
    writeData(next);
    freeImage(curr);
    freeImage(next);
    return 0;
}
