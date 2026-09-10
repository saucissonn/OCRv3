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
    Rectangle *rectangles = NULL;

    Rectangle *default_background = create_rectangle(0, 0, 1, 1, 0, White);
    rectangles = add_rectangle(rectangles, default_background);

    return rectangles;
}

static Text *init_texts()
{
    Text *res = NULL;

    Text *txt1 = create_text(
        "Thanks to everyone",
        0.5,
        0.5,
        FONT_SIZE5,
        Black
    );

    res = add_text(res, txt1);

    return res;
}

static Button *init_buttons(Frame *frame)
{
    Button *res = NULL;

	Button *btn1 = create_options_button(frame);

    res = add_button(res, btn1);

    return res;
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
    Frame *frame = data[0];

    clear_frame(frame);
}

void init_frame_credits(Frame *frame)
{
    frame->rectangles = init_rectangles();
    frame->texts = init_texts();
    frame->buttons = init_buttons(frame);
    frame->text_areas = init_text_areas();
    frame->cursors = init_cursors();

    frame->quit_function = quit_function;
    frame->quit_args = malloc(sizeof(void *) * 2);
    frame->quit_args[0] = frame;
    frame->quit_args[1] = NULL;
}

