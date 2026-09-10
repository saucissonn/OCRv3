#include "rectangle.h"
#include "../globals.h"
#include "../utils.h"

#include <SDL2/SDL.h>

Rectangle *create_rectangle(float x, float y, float w, float h, float outline_w, SDL_Color color) {
    Rectangle *res = malloc(sizeof(Rectangle));

	res->x = x;
	res->y = y;
	res->w = w;
	res->h = h;

	SDL_Rect *outline = malloc(sizeof(SDL_Rect));;

	if (outline_w <= 0) {
		outline_w = 0.0;
	}

	res->outline_w = outline_w;
	res->outline = outline;
	
    SDL_Rect *rect = malloc(sizeof(SDL_Rect));
    rect->x = (x + outline_w) * DELTA_W;
	rect->y = (y + outline_w) * DELTA_H;
    rect->w = (w - outline_w * 2) * DELTA_W;
    rect->h = (h - outline_w * 2) * DELTA_H;

	if (outline_w) {
		outline->x = x * DELTA_W;
        outline->y = y * DELTA_H;
        outline->w = w * DELTA_W;
        outline->h = h * DELTA_H;
	}
	else {
        outline->x = rect->x;                                                                                       
        outline->y = rect->y;                                                                                       
        outline->w = rect->w;                                                                                       
        outline->h = rect->h;
	}

    res->color = color;
    res->rect = rect;
    res->next = NULL;

	return res;
}

Rectangle *copy_rectangle(Rectangle *rect) {
    if (!rect) {
        return NULL;
    }

    return create_rectangle(
        rect->x,
        rect->y,
        rect->w,
        rect->h,
        rect->outline_w,
        rect->color
    );
}

Rectangle *add_rectangle(Rectangle *rectangles, Rectangle *rectangle) {
	if (!rectangles) return rectangle;

	Rectangle *curr = rectangles;
	
	while (curr->next) {
		curr = curr->next;
	}

	curr->next = rectangle;
	return rectangles;
}

void display_rectangles(SDL_Renderer *renderer, Rectangle *rectangles, int over) {
	Rectangle *curr = rectangles;

	while (curr) {
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderFillRect(renderer, curr->outline);

		SDL_Color c = curr->color;
		SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);						
		SDL_RenderFillRect(renderer, curr->rect);
		curr = curr->next;
	}
}
void destroy_rectangles(Rectangle *rectangles) {
    Rectangle *curr = rectangles;

    while (curr) {
		Rectangle *temp = curr;
		free(curr->outline);
		free(curr->rect);
		curr = curr->next;
		free(temp);
    }
}

void shift_down_rectangle(int nb, Rectangle *rectangle) {
	if (rectangle->outline) {
		rectangle->outline->y += nb * rectangle->outline->h;
        rectangle->rect->y += nb * rectangle->outline->h;
	}
	else {
		rectangle->rect->y += nb * rectangle->rect->h;
	}
	rectangle->y += nb * rectangle->h;
}
