#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "image.h"

void rotate_image(Image *src, double angle);

Image *crop_white(Image *src);

double hough_angle(Image *img, double precision);

void modify_sudoku_image(Image *img, int *empty_sudoku, int *solved_sudoku);

#endif
