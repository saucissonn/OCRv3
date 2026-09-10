#ifndef UTILS_H
#define UTILS_H

#include <SDL2/SDL.h>

int collision_rect(int x, int y, SDL_Rect rect);

void init_global_variables();
char *itoa_base_n(int x, int n);

#endif
