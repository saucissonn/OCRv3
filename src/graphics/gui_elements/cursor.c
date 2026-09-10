#include "cursor.h"
#include "text.h"
#include "rectangle.h"
#include "../utils.h"
#include "../frame.h"
#include "../globals.h"

#include <math.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>

Cursor *create_cursor(float x, float y, float w, float h, float min, float max) {
	Cursor *res = malloc(sizeof(Cursor));

	res->min = min;
    res->max = max;
    res->value = 0;

    res->background = create_rectangle(x, y, w, h, DEFAULT_SPACE_W / 2., White);
    res->loading = create_rectangle(x, y, h, h, DEFAULT_SPACE_W / 2., Blue);
	res->dragzone = create_rectangle(x, y, h, h, DEFAULT_SPACE_W / 2., Green);

	char str[100];
	sprintf(str, "%.2f", min);

    res->min_text = create_text(str, x, y + h, 0.03, Black);

    char str2[100];
    sprintf(str2, "%.2f", max);
    res->max_text = create_text(str2, x + w, y + h, 0.03, Black);
    res->val_text = create_text(str, x, y, 0.03, Black);
	res->val_text->y -= res->val_text->rect->h / DELTA_H;
	res->val_text->rect->y -= res->val_text->rect->h;

	res->next = NULL;

	return res;
}

Cursor *add_cursor(Cursor *cursors, Cursor *cursor) {
	if (!cursors)
		return cursor;

	Cursor *curr = cursor;

    while (curr->next) {
        curr = curr->next;
    }

    curr->next = cursor;

    return cursors;
}

void display_cursors(SDL_Renderer *renderer, Cursor *cursors) {
	Cursor *curr = cursors;

    while (curr) {
        display_rectangles(renderer, curr->background, 0);
		display_rectangles(renderer, curr->loading, 0);
		display_rectangles(renderer, curr->dragzone, 1);

        display_texts(curr->min_text);
        display_texts(curr->max_text);
        display_texts(curr->val_text);
    
        curr = curr->next;
    }	
}

void destroy_cursors(Cursor *cursors) {
	Cursor *curr = cursors;

	while (curr) {
		destroy_rectangles(curr->background);
		destroy_rectangles(curr->loading);
		destroy_rectangles(curr->dragzone);

		destroy_texts(curr->min_text);
        destroy_texts(curr->max_text);
        destroy_texts(curr->val_text);
		
		Cursor *temp = curr;
		curr = curr->next;

		free(temp);
	}
}

Cursor *collision_cursors(int x, int y, Cursor *cursors) {
	Cursor *curr =cursors;

    while (curr) {
        if (collision_rect(x, y, *curr->background->rect)) {
            return curr;
        }
        curr = curr->next;
    }

    return NULL;
}

void modify_pos_cursor(int x, int y, Cursor *cursor) { // Place the dragzone at the pos x and y
    if (!left_pressed || !cursor) return;

	// Modify pos
    cursor->dragzone->x += (float)x / DELTA_W - cursor->dragzone->x - (cursor->dragzone->rect->w + cursor->background->outline_w) / (2 * DELTA_W);
    cursor->dragzone->rect->x += x - cursor->dragzone->rect->x - cursor->dragzone->rect->w / 2;

    if (cursor->dragzone->outline) {
        cursor->dragzone->outline->x += x - cursor->dragzone->outline->x - cursor->dragzone->outline->w / 2;
    }

	// Check if out of the box
	// Left
    if (cursor->dragzone->x < cursor->background->x + cursor->background->outline_w) {
        cursor->dragzone->x = cursor->background->x + cursor->background->outline_w;
        cursor->dragzone->rect->x = cursor->dragzone->x * DELTA_W;
        if (cursor->dragzone->outline) {
            cursor->dragzone->outline->x = cursor->background->x * DELTA_W;
        }
    }
	//Right
    else if (cursor->dragzone->x > cursor->background->x + cursor->background->w - cursor->dragzone->w + cursor->background->outline_w) {
        cursor->dragzone->x = cursor->background->x + cursor->background->w - cursor->dragzone->w + cursor->background->outline_w;
        cursor->dragzone->rect->x = cursor->dragzone->x * DELTA_W;
        if (cursor->dragzone->outline) {
            cursor->dragzone->outline->x = (cursor->background->x + cursor->background->w - cursor->dragzone->w) * DELTA_W;
        }
    }

	// Modify loading bar
    cursor->loading->w = cursor->dragzone->x - cursor->background->x + cursor->dragzone->w - 2 * cursor->dragzone->outline_w;
    cursor->loading->rect->w = cursor->loading->w * DELTA_W;
}

float get_value_cursor(Cursor *cursor) {
	float pos = cursor->dragzone->x - cursor->background->x - cursor->background->outline_w;

	float res = pos / (cursor->background->w - cursor->dragzone->w) * (cursor->max - cursor->min);
	if (res < cursor->min)
		res = cursor->min;
	if (res > cursor->max)
		res = cursor->max;
	//res = roundf(res * 100.0f) / 100.0f;

	return res;
}
