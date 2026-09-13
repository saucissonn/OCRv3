#ifndef TEXT_AREA_H
#define TEXT_AREA_H

#include "text.h"
#include "rectangle.h"

typedef struct TextArea {
	Text *text;
	Rectangle *rect;

	int digits; // Can contains digits
	int alpha;	// Can contains alpha and '_'

	struct TextArea *next;
} TextArea;

void lower_text(Text *text, Rectangle *rectangle);

TextArea *create_text_area(Text *text, Rectangle *rect);
TextArea *add_text_area(TextArea *text_areas, TextArea *text_area);

void display_text_areas(SDL_Renderer *renderer, TextArea *text_areas);
void destroy_text_areas(TextArea *text_areas);
TextArea *collision_text_areas(int x, int y, TextArea *text_areas);

void display_blinker(SDL_Renderer *renderer, TextArea *text_area);

#endif
