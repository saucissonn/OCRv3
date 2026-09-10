#ifndef MATRIX_H
#define MATRIX_H

#include "../process_img/image.h"

int *coordinates_to_matrix(Image *img, int x0, int y0, int x1, int y1);
int *matrix_to_28x28(int *matrix, int w, int h);

void print_matrix(int *matrix, int w, int h);
void print_matrix_values(int *matrix, int w, int h);

#endif
