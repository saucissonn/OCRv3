#include "../frame.h"
#include "../gui_elements/rectangle.h"
#include "../gui_elements/text.h"
#include "../gui_elements/button.h"
#include "../gui_elements/text_area.h"
#include "../image_graphics.h"
#include "../globals.h"
#include "../update.h"
#include "common_elements.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

static void swap_frame(void **data)
{
	printf("Swapped\n");

	Frame *frame = data[0];
	void (*function)(Frame *) = data[1];

	if (!frame->quit_function)
	{
		perror("Can't quit the current frame");
		return;
	}

	frame->quit_function(frame->quit_args); // Quit the current frame (often clear the frame)
	function(frame); // Execute the function (often init a new frame)
}

void option_button_function(void **data)
{
    printf("SUB1\n");
    Button *button = data[0];
    Frame *frame = data[1];

    Frame *frame2 = create_frame(frame);
    Button *sub_buttons = create_submenu(4, button);

    Button *curr = sub_buttons;
    
	modify_text(Renderer, curr->text, "OCR");
    center_text(curr->text, curr->rect);

    curr->function = swap_frame;
	curr->args = malloc(sizeof(void *) * 3);
	curr->args[0] = frame;
	curr->args[1] = init_frame_ocr;
	curr->args[2] = NULL;

	curr = curr->next;

    modify_text(Renderer, curr->text, "TRAINING");
    center_text(curr->text, curr->rect);

    curr->function = swap_frame;
    curr->args = malloc(sizeof(void *) * 3);
    curr->args[0] = frame;
    curr->args[1] = init_frame_training;
    curr->args[2] = NULL;

	curr = curr->next;

    modify_text(Renderer, curr->text, "TUTORIAL");
    center_text(curr->text, curr->rect);
    
    curr->function = swap_frame;
    curr->args = malloc(sizeof(void *) * 3);
    curr->args[0] = frame;
    curr->args[1] = init_frame_tutorial;
    curr->args[2] = NULL;

	curr = curr->next;

    modify_text(Renderer, curr->text, "CREDITS");
    center_text(curr->text, curr->rect);

    curr->function = swap_frame;
    curr->args = malloc(sizeof(void *) * 3);
    curr->args[0] = frame;
    curr->args[1] = init_frame_credits;
    curr->args[2] = NULL;
    
	curr = curr->next;

    frame2->buttons = sub_buttons;

	resize_buttons(Renderer, frame2->buttons);

	frame->subframe = frame2;
}

Button *create_options_button(Frame *frame)
{
    Text *text = create_text("OPTIONS", 0, 0, FONT_SIZE3, White);
    Rectangle *rect = create_rectangle(0.25, 0, 0.1, 0.05, DEFAULT_BORDER_W, Green);
    Button *btn = create_button(1, text, rect);

    btn->function = option_button_function;
    btn->args = malloc(sizeof(void *) * 3);
    btn->args[0] = btn;
    btn->args[1] = frame;
    btn->args[2] = NULL;

	return btn;
}
