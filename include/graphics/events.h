#ifndef EVENTS_H
#define EVENTS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "frame.h"
#include "gui_elements/button.h"

void handle_event_button(Button *button);
void handle_event_text_area(TextArea *text_area);
void handle_event_cursor(Cursor *cursor);

void handle_event_nontext_key(SDL_Event event, Frame *frame);
int handle_event_mouse(Frame *frame);

int handle_events(SDL_Event event, Frame *frame);

#endif
