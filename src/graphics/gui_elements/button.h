#ifndef BUTTON_H
#define BUTTON_H

#include "text.h"
#include "rectangle.h"

#include <SDL2/SDL.h>

typedef struct Frame Frame;

typedef struct Button {
	int interactible;
	Text *text;
	Rectangle *rect;
	void (*function)(void **);
	void **args;
	struct Button *next;
} Button;

void center_text(Text *text, Rectangle *rectangle);

Button *create_button(int interactible, Text *text, Rectangle *rect);
Button *add_button(Button *buttons, Button *button);
Button *copy_button(Button *button);

void display_buttons(SDL_Renderer *renderer, Button *buttons);
void destroy_buttons(Button *buttons);
Button *collision_buttons(int x, int y, Button *buttons);

Button *create_submenu(int nb, Button *button);

#endif
