#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "image.h"

void rotate_image(Image *src, double angle);

Image *crop_white(Image *src);

double hough_angle(Image *img, double precision);

void modify_image(Image *img, char *empty, char *solved);

#endif
