#ifndef FRAME_H
#define FRAME_H

#include <SDL2/SDL.h>

#include "gui_elements/rectangle.h"
#include "gui_elements/text.h"
#include "gui_elements/text_area.h"
#include "gui_elements/cursor.h"
#include "../ocr/process_img/image.h"

typedef struct Button Button;
typedef struct Cursor Cursor;

typedef struct Frame {
	char *name;

	Image *image;
    SDL_Texture *texture; // Texture of the image
    int image_w; // w and h of the texture
    int image_h;

	int drop_zone; // 1 or 0

	// UI of the frame
	Rectangle *rectangles;
	Text *texts;
	Button *buttons;
	TextArea *text_areas;
	TextArea *current_text_area;
	Cursor *cursors;
	Cursor *current_cursor;

	// Special functions
    void (*quit_function)(void **); // Called at the exit of the frame
    void **quit_args;

    void (*additional_display_function)(void **); // To add dynamic / complex display
    void **additional_display_args;

	struct Frame *parentframe;
	struct Frame *subframe;
} Frame;


Frame *create_frame(Frame *parent);

void clear_current_states(Frame *frame);
void clear_frame(Frame *frame);

void destroy_frame(Frame *frame);

void display_frame(Frame *frame);

#endif
