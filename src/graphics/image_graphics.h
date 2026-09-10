#ifndef IMAGE_GRAPHICS_H
#define IMAGE_GRAPHICS_H

#include "frame.h"

int load_image(Frame *frame, const char *path);
void render_image(Frame *frame);

#endif
