#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "imageloader.h"

//Determines what color the cell at the given row/col should be. This should not affect Image, and should allocate space for a new Color.
Color *evaluateOnePixel(Image *image, int row, int col)
{
	Color *newColor = (Color *) malloc(sizeof(Color));
	if (newColor == NULL) {
		return NULL;
	}
	Color **colors = image->image;
	Color eval = colors[row][col];
	uint8_t blue = eval.B;
	if (blue & 1) {
		newColor->R = 255;
		newColor->G = 255;
		newColor->B = 255;
	} else {
		newColor->R = 0;
		newColor->G = 0;
		newColor->B = 0;
	}

	return newColor;
}

//Given an image, creates a new image extracting the LSB of the B channel.
Image *steganography(Image *image)
{
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
			Color * color_pointer = evaluateOnePixel(image, i, j);
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
Loads a file of ppm P3 format from a file, and prints to stdout (e.g. with printf) a new image, 
where each pixel is black if the LSB of the B channel is 0, 
and white if the LSB of the B channel is 1.

argc stores the number of arguments.
argv stores a list of arguments. Here is the expected input:
argv[0] will store the name of the program (this happens automatically).
argv[1] should contain a filename, containing a file of ppm P3 format (not necessarily with .ppm file extension).
If the input is not correct, a malloc fails, or any other error occurs, you should exit with code -1.
Otherwise, you should return from main with code 0.
Make sure to free all memory before returning!
*/
int main(int argc, char **argv)
{
	if (argc != 2) {
		return -1;
	}
	Image *curr = readData(argv[1]);
	if (curr == NULL) {
		return -1;
	}

	Image *res = steganography(curr);
	
	if (res == NULL) {
		freeImage(curr);
		return -1;
	}

	writeData(res);

	freeImage(curr);
	freeImage(res);
	return 0;
}
