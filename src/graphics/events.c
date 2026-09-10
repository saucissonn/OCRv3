#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <ctype.h>

#include "globals.h"
#include "gui_elements/button.h"
#include "frame.h"
#include "image_graphics.h"
#include "../ocr/process_img/image.h"

void handle_event_button(Button *button) {
	if (button->interactible && button->function) {
		button->function(button->args);
	}
}

void handle_event_text_area(TextArea *text_area) {
	printf("Text area event\n");
}

void handle_event_cursor(Cursor *cursor) {
	printf("Cursor event\n");
}

void handle_event_nontext_key(SDL_Event event, Frame *frame) {
	switch(event.key.keysym.sym) {
		case SDLK_RIGHT:
			printf("Right key pressed\n");
			break;

		case SDLK_BACKSPACE:
			if (frame->current_text_area) {
				delete_char(Renderer, frame->current_text_area->text);
			}
			break;

		case SDLK_RETURN: // Press Enter
			break;
	}
}

int handle_event_mouse(Frame *frame) {
	int mouseX, mouseY;
	switch(SDL_GetMouseState(&mouseX, &mouseY)) {
		case SDL_BUTTON(SDL_BUTTON_LEFT):
			clear_current_states(frame);
			left_pressed = 1;

			Button *btn_clicked = collision_buttons(mouseX, mouseY, frame->buttons);
			if (btn_clicked) {
				handle_event_button(btn_clicked);
                return 1;
			}

			frame->current_text_area = collision_text_areas(mouseX, mouseY, frame->text_areas);
			if (frame->current_text_area)
			{
				SDL_StartTextInput();

				handle_event_text_area(frame->current_text_area);
				return 1;
			}

			SDL_StopTextInput();

			frame->current_cursor = collision_cursors(mouseX, mouseY, frame->cursors);
			if (frame->current_cursor) {
				handle_event_cursor(frame->current_cursor);
				return 1;
			}

			//printf("Left click pressed at pos: %d, %d\n", mouseX, mouseY);
			break;		

		default:
			printf("Mouse click pressed at pos: %d, %d\n", mouseX, mouseY);
			break;
	}
	return 0;
}

int handle_events(SDL_Event event, Frame *frame) {
	while (SDL_PollEvent(&event))
	{
		switch(event.type) {
			case SDL_QUIT:
				running = 0;
				break;

			case SDL_DROPFILE:
			{
				char *path = event.drop.file;

				if (frame->drop_zone == 0)
				{
					SDL_free(path); // We have to free it even if no file is displayed
					break;
				}

				printf("File: %s\n", path);
				Image *img = load_png(path);
				//process_image(img);

				free_image(frame->image);
				frame->image = img;

				frame->texture = image_to_texture(Renderer, img);

				SDL_QueryTexture(frame->texture, NULL, NULL, &frame->image_w, &frame->image_h);

				SDL_free(path);
				break;
			}

			case SDL_TEXTINPUT: // Press a symbol
				if (frame->current_text_area) {
					char c = event.text.text[0];
					if (frame->current_text_area->digits && isdigit(c) ||
						frame->current_text_area->alpha && (isalpha(c) || c == '_'))
					{
						TTF_Font *font = TTF_OpenFont("graphics/DejaVuSans.ttf", (int)(frame->current_text_area->text->size * DELTA));

						char buff[2];
						buff[0] = c;
						buff[1] = '\0';

					    SDL_Surface *surface_temp = TTF_RenderUTF8_Blended(font, buff, White);

						int size_element = surface_temp->w;

						SDL_FreeSurface(surface_temp);
						TTF_CloseFont(font);

						if (size_element + frame->current_text_area->text->rect->w < frame->current_text_area->rect->rect->w - DEFAULT_BORDER_W * DELTA_W)
							add_char(Renderer, frame->current_text_area->text, c);
					}
				}
				printf("char is: %s\n", event.text.text);
				break;

			case SDL_KEYDOWN: // Press a key but not a symbol
				handle_event_nontext_key(event, frame);
				break;

			case SDL_MOUSEBUTTONDOWN:
			{
				Frame *curr = frame;

				while (curr->subframe) {
					curr = curr->subframe;
				}

				while (curr) {
					if (handle_event_mouse(curr)) {
						mouse_used = 1;
						return 1;
					}

					if (curr != frame) {
						Frame *parent = frame;

						while (parent->subframe != curr) {
							parent = parent->subframe;
						}

						destroy_frame(curr);
						parent->subframe = NULL;

						curr = parent;
					} else {
						break;
					}
				}

				return 0;
			}
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    frame->current_cursor = NULL;
					left_pressed = 0;
				}
                break;
		}
	}
	return 0;
}
