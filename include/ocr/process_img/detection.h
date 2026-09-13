#ifndef DETECTION_H
#define DETECTION_H

#include "ocr/process_img/image.h"

ImageRect **get_rectangles(Image *img);

ImageRect *dfs(Image *img, int x, int y, uint8_t *visited);

ImageRect **get_puzzle_rectangles(Image *img);
ImageRect **get_word_rectangles(Image *img);

int *get_bbox(int *matrix, int w, int h, int *x0, int *y0, int *x1, int *y1);

void debug_puzzle_rectangles(Image *img);
void mask_puzzle_rectangles(Image *img, ImageRect *rect);

#endif
