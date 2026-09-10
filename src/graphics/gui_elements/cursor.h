#ifndef CURSOR_H
#define CURSOR_H

#include "text.h"
#include "rectangle.h"
#include "../frame.h"

#include <SDL2/SDL.h>

typedef struct Frame Frame;

typedef struct Cursor {
	float min;
	float max;
	float value;
	
	Rectangle *background;
	Rectangle *loading;
	Rectangle *dragzone;

	Text *min_text;
	Text *max_text;
	Text *val_text;
	
	struct Cursor *next;
} Cursor;

Cursor *create_cursor(float x, float y, float w, float h, float min, float max);
Cursor *add_cursor(Cursor *cursors, Cursor *cursor);

void display_cursors(SDL_Renderer *renderer, Cursor *cursors);
void destroy_cursors(Cursor *cursors);

Cursor *collision_cursors(int x, int y, Cursor *cursors);

void modify_pos_cursor(int x, int y, Cursor *cursor);
float get_value_cursor(Cursor *cursor);

#endif
