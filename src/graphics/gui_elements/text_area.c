#include "text_area.h"
#include "../utils.h"
#include "../globals.h"

#include <SDL2/SDL.h>

void lower_text(Text *text, Rectangle *rectangle) {
    if (!text || !rectangle) return;

    float dy = rectangle->h - (text->rect->h) / (float)DELTA_H;

    text->x = rectangle->x + rectangle->outline_w;
    text->y = rectangle->y - rectangle->outline_w + dy;
    text->rect->x = text->x * DELTA_W;
    text->rect->y = text->y * DELTA_H;
}

TextArea *create_text_area(Text *text, Rectangle *rect) {
    TextArea *res = malloc(sizeof(TextArea));

	text->len_max = (int)(rect->w / text->size_one_element); // Update new limit
    res->text = text;
    res->rect = rect;

	lower_text(res->text, res->rect);

	res->digits = 1; // Default value
	res->alpha = 1; // Default value

    res->next = NULL;

    return res;
}

TextArea *add_text_area(TextArea *text_areas, TextArea *text_area) {
    if (!text_areas) return text_area;

    TextArea *curr = text_areas;

    while (curr->next) {
        curr = curr->next;
    }

    curr->next = text_area;

    return text_areas;
}

TextArea *init_text_areas() {
    TextArea *res = NULL;

    Text *text1 = create_text("Area1", 0, 0, 0.05, Black);
    Rectangle *rect1 = create_rectangle(DEFAULT_SPACE_W, 0.2, 0.3, 0.1, DEFAULT_BORDER_W, White);

    TextArea *txta1 = create_text_area(text1, rect1);

    res = add_text_area(res, txta1);

    Text *text2 = create_text("Area1", 0, 0, 0.05, Black);
    Rectangle *rect2 = create_rectangle(DEFAULT_SPACE_W, 0.50, 0.3, 0.1, DEFAULT_BORDER_W, White);

    TextArea *txta2 = create_text_area(text2, rect2);

    res = add_text_area(res, txta2);

    return res;	
}

void display_text_areas(SDL_Renderer *renderer, TextArea *text_areas) {
    TextArea *curr = text_areas;

    while (curr) {
        display_rectangles(renderer, curr->rect, 2);
        display_texts(curr->text);
        curr = curr->next;
    }

}

void destroy_text_areas(TextArea *text_areas) {
    TextArea *curr = text_areas;

    while (curr) {
        TextArea *temp = curr;
        destroy_texts(curr->text);
        destroy_rectangles(curr->rect);
        curr = curr->next;
        free(temp);
    }
	
}

TextArea *collision_text_areas(int x, int y, TextArea *text_areas) {
	TextArea *curr = text_areas;	

    while (curr) {
        if (collision_rect(x, y, *curr->rect->rect)) {
            return curr;
        }
        curr = curr->next;
    }

    return NULL;
}

void display_blinker(SDL_Renderer *renderer, TextArea *text_area) {
	if (!text_area) return;
	 
	SDL_Rect rect = {
		.x = text_area->text->rect->x + text_area->text->rect->w,
		.y = text_area->text->rect->y,
		.w = DEFAULT_BORDER_W * DELTA_W,
		.h = text_area->text->rect->h
	};

	int average = 0;
	average += text_area->rect->color.r;
	average += text_area->rect->color.g;
	average += text_area->rect->color.b;
	average /= 3;
	
	if (average < 70) {
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	}
	else {
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	}
	SDL_RenderFillRect(renderer, &rect);
}
