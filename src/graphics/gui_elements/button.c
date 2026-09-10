#include "button.h"
#include "text.h"
#include "rectangle.h"
#include "../utils.h"
#include "../frame.h"
#include "../globals.h"

#include <SDL2/SDL_ttf.h>
#include <string.h>

Button *create_submenu(int nb, Button *button);

void center_text(Text *text, Rectangle *rectangle) {
    if (!text || !rectangle) return;

    float dx = (rectangle->w - (text->rect->w) / (float)DELTA_W) / 2;
    float dy = (rectangle->h - (text->rect->h) / (float)DELTA_H) / 2;

    text->x = rectangle->x + dx;
    text->y = rectangle->y + dy;
    text->rect->x = text->x * DELTA_W;
    text->rect->y = text->y * DELTA_H;
}

Button *create_button(int interactible, Text *text, Rectangle *rect) {
	Button *res = malloc(sizeof(Button));

	res->interactible = interactible;
    text->len_max = 10000; // Update new limit, not accessible by the client
	res->text = text;
	res->rect = rect;

	center_text(res->text, res->rect);
	
	res->function = NULL;
	res->args = NULL;

	res->next = NULL;

	return res;
}

Button *copy_button(Button *button) { // Copy everything from a button except its actions
    if (!button) {
        return NULL;
    }

    Button *res = create_button(
        button->interactible,
        copy_text(button->text),
        copy_rectangle(button->rect)
    );

    res->function = NULL;
    res->args = NULL;

    return res;
}

Button *add_button(Button *buttons, Button *button) {
	if (!buttons) return button;

	Button *curr = buttons;

	while (curr->next) {
		curr = curr->next;
	}
	
	curr->next = button;

	return buttons;
}

void display_buttons(SDL_Renderer *renderer, Button *buttons) {
	Button *curr = buttons;

	while (curr) {
		display_rectangles(renderer, curr->rect, 1);
		display_texts(curr->text);
		curr = curr->next;
	}
}

void destroy_buttons(Button *buttons) {
    Button *curr = buttons;

    while (curr) {
		Button *temp = curr;
		destroy_texts(curr->text);
		destroy_rectangles(curr->rect);
		free(curr->args);
		curr = curr->next;
		free(temp);	
	}
}

Button *collision_buttons(int x, int y, Button *buttons) {
    Button *curr = buttons;

    while (curr) {
		if (collision_rect(x, y, *curr->rect->outline)) {
			return curr;
		}
		curr = curr->next;
	}

	return NULL;
}

Button *create_submenu(int nb, Button *button) { // Create nb copies of a button from top to bottom
	Button *res = NULL;	

	for (int i = 1; i <= nb; i++) {
		Button *copy = copy_button(button);
		
		shift_down_rectangle(i, copy->rect);
		center_text(copy->text, copy->rect);

		res = add_button(res, copy);
	}

	return res;
}
