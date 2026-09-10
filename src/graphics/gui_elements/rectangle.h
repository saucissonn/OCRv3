#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <SDL2/SDL.h>

typedef struct Rectangle {
	float x;
	float y;
	float w;
	float h;

	float outline_w;

    SDL_Color color;
	SDL_Rect *outline;
    SDL_Rect *rect;

    struct Rectangle *next;
} Rectangle;

Rectangle *create_rectangle(float x, float y, float w, float h, float outline_w, SDL_Color color);
Rectangle *copy_rectangle(Rectangle *rect);

Rectangle *add_rectangle(Rectangle *rectangles, Rectangle *rectangle);

void display_rectangles(SDL_Renderer *renderer, Rectangle *rectangles, int over);
void destroy_rectangles(Rectangle *rectangles);

void shift_down_rectangle(int nb, Rectangle *rectangle);

#endif
