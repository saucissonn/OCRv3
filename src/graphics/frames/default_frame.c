// The only purpose of this file is to be copy and paste to simplify the process of creating a new frame

#include "../frame.h"
#include "../gui_elements/rectangle.h"
#include "../gui_elements/text.h"
#include "../gui_elements/button.h"
#include "../gui_elements/text_area.h"
#include "../image_graphics.h"
#include "../globals.h"
#include "frames.h"
#include "common_elements.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

static Rectangle *init_rectangles() 
{
	return NULL;
}

static Text *init_texts()
{
	return NULL;
}

static Button *init_buttons(Frame *frame)
{
	return NULL;
}

static TextArea *init_text_areas()
{
	return NULL;
}

static Cursor *init_cursors()
{
	return NULL;
}

static void quit_function(void **data)
{
	return;
}

void init_frame_default(Frame *frame)
{
	frame->rectangles = init_rectangles();
	frame->texts = init_texts();
	frame->buttons = init_buttons(frame);	
	frame->text_areas = init_text_areas();
	frame->cursors = init_cursors();
	
	frame->quit_args = NULL;
	frame->quit_function = quit_function;
}

