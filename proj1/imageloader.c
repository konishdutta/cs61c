#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "imageloader.h"

//Opens a .ppm P3 image file, and constructs an Image object. 
//You may find the function fscanf useful.
//Make sure that you close the file with fclose before returning.
Image *readData(char *filename) 
{
	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		return NULL;
	}
	uint32_t width;
	uint32_t height;
	fscanf(fp, "%*s");
	fscanf(fp, "%u %u", &width, &height);
	fscanf(fp, "%*u");

	Color **res = (Color **) malloc(sizeof(Color *) * height);

	if (res == NULL) {
		fclose(fp);
		return NULL;
	}

	for (int i = 0; i < height; i++) {
		res[i] = (Color *) malloc(sizeof(Color) * width);
		if (res[i] == NULL) {
			for (int j = 0; j < i; j++) {
				free(res[j]);
			}
			free(res);
			fclose(fp);
			return NULL;
		}
		for (int k = 0; k < width; k++) {
			fscanf(fp, "%hhu %hhu %hhu", &(res[i][k].R), &(res[i][k].G), &(res[i][k].B));
		}
	}

	fclose(fp);

	Image *val = (Image *) malloc(sizeof(Image));
	if (val == NULL) {
		for (int i = 0; i < height; i++) {
			free(res[i]);
		}
		free(res);
		return NULL;
	}

	val->image = res;
	val->rows = height;
	val->cols = width;
	return val;
}

//Given an image, prints to stdout (e.g. with printf) a .ppm P3 file with the image's data.
void writeData(Image *image)
{
	//header
	printf("P3\n");
	printf("%d %d\n", image->cols, image->rows);
	printf("255\n");
	//image
	Color ** curr= image->image;
	if (curr == NULL) {
		return;
	}
	for (int i = 0; i < image->rows; i++) {
		for (int j = 0; j < image->cols; j++) {
			if (j != 0) {
				printf("   ");
			}
    		printf("%3d %3d %3d", curr[i][j].R, curr[i][j].G, curr[i][j].B);
		}
		printf("\n");
	}
}

//Frees an image
void freeImage(Image *image)
{
	for (int i = 0; i < image->rows; i++) {
		free(image->image[i]);
	}
	free(image->image);
	free(image);
}