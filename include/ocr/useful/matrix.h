#ifndef MATRIX_H
#define MATRIX_H

#include "ocr/process_img/image.h"

int *coordinates_to_matrix(Image *img, int x0, int y0, int x1, int y1);
int *matrix_to_28x28(int *matrix, int w, int h);

uint8_t **expand_matrix(uint8_t **matrix, int w, int h, int new_w, int new_h);
void shift_matrix(uint8_t **matrix, int w, int h, int shift_x, int shift_y);

void print_matrix(int *matrix, int w, int h);
void print_matrix_values(int *matrix, int w, int h);

// TESTS

void test_shift_matrix_right(void);
void test_shift_matrix_left(void);
void test_shift_matrix_down(void);
void test_shift_matrix_up(void);
void test_shift_matrix_diagonal(void);
void test_shift_matrix_no_shift(void);

void test_expand_matrix(void);

void run_matrix_tests(void);

#endif
