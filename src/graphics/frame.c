#include "frame.h"
#include "gui_elements/rectangle.h"
#include "gui_elements/text.h"
#include "gui_elements/button.h"
#include "gui_elements/text_area.h"
#include "image_graphics.h"
#include "globals.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

Frame *create_frame(Frame *parent)
{
	Frame *frame = malloc(sizeof(Frame));

	frame->name = NULL;

	frame->image = NULL;
    frame->texture = NULL;
    frame->image_w = 0;
    frame->image_h = 0;

	frame->drop_zone = 0;

    frame->rectangles = NULL;
    frame->texts = NULL;
    frame->buttons = NULL;
    frame->text_areas = NULL;
    frame->current_text_area = NULL;
	frame->cursors = NULL;
	frame->current_cursor = NULL;

	frame->quit_function = NULL;
	frame->quit_args = NULL;

	frame->additional_display_function = NULL;
	frame->additional_display_args = NULL;

	frame->parentframe = parent;
    frame->subframe = NULL;

	if (parent)
		parent->subframe = frame;

    return frame;
}

void clear_current_states(Frame *frame) // Important when the user clicks to have one interaction at the time
{
	frame->current_text_area = NULL;
	frame->current_cursor = NULL;
}

void clear_frame(Frame *frame)
{
	free(frame->name);
	frame->name = NULL;

	free_image(frame->image);
	frame->image = NULL;

	frame->texture = NULL;
    frame->image_w = 0;
    frame->image_h = 0;

	frame->drop_zone = 0;

	destroy_rectangles(frame->rectangles);
    frame->rectangles = NULL;

	destroy_texts(frame->texts);
    frame->texts = NULL;

	destroy_buttons(frame->buttons);
    frame->buttons = NULL;

	destroy_text_areas(frame->text_areas);
    frame->text_areas = NULL;
    frame->current_text_area = NULL;

	destroy_cursors(frame->cursors);
	frame->cursors = NULL;

    frame->quit_function = NULL;

	free(frame->quit_args);
    frame->quit_args = NULL;

    frame->additional_display_function = NULL;

	free(frame->additional_display_args);
    frame->additional_display_args = NULL;

	frame->parentframe = NULL;

	clear_current_states(frame);

	destroy_frame(frame->subframe);
    frame->subframe = NULL;
}

void destroy_frame(Frame *frame)
{
	if (!frame)
		return;

	if (frame->texture)
		SDL_DestroyTexture(frame->texture);

	if (frame->quit_function)
		frame->quit_function(frame->quit_args);

	clear_frame(frame);

	destroy_frame(frame->subframe);

	free(frame);
}

void display_frame(Frame *frame) 
{
	if (!frame)
		return;

    display_rectangles(Renderer, frame->rectangles, 0);
    display_texts(frame->texts);
    display_buttons(Renderer, frame->buttons);
    display_text_areas(Renderer, frame->text_areas);
    display_blinker(Renderer, frame->current_text_area);
	display_cursors(Renderer, frame->cursors);

	render_image(frame);

	if (frame->additional_display_function)
		frame->additional_display_function(frame->additional_display_args);

	display_frame(frame->subframe);
}
