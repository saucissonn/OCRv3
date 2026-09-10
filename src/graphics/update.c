#include "globals.h"
#include "gui_elements/rectangle.h"
#include "gui_elements/text.h"
#include "gui_elements/button.h"
#include "update.h"
#include "gui_elements/cursor.h"
#include "utils.h"
#include "globals.h"

#include <SDL2/SDL.h>
#include <stdio.h>

void update_mouse() {
	if (mouse_hover == 0)
		SDL_SetCursor(arrowCursor);
	else if (mouse_hover == 1)
		SDL_SetCursor(handCursor);
	else if (mouse_hover == 2)
		SDL_SetCursor(textCursor);
	SDL_ShowCursor(SDL_ENABLE);	
	mouse_hover = 0;
	mouse_used = 0;
}

void resize_rectangles(Rectangle *rectangles) {
	Rectangle *curr = rectangles;

	while (curr) {
		if (curr->outline) {
	        curr->outline->x = curr->x * DELTA_W;
	        curr->outline->y = curr->y * DELTA_H;
	        curr->outline->w = curr->w * DELTA_W;
	        curr->outline->h = curr->h * DELTA_H;	
		}
		
	    curr->rect->x = (curr->x + curr->outline_w) * DELTA_W;
	    curr->rect->y = (curr->y + curr->outline_w) * DELTA_H;
	    curr->rect->w = (curr->w - curr->outline_w * 2) * DELTA_W;
	    curr->rect->h = (curr->h - curr->outline_w * 2) * DELTA_H;

		curr = curr->next;
	}
}

void update_rectangles(Frame *frame) {
	printf("Update rectangles\n");
}

void resize_texts(SDL_Renderer *renderer, Text *texts) {
    Text *curr = texts;

    while (curr) {
        curr->rect->x = curr->x * DELTA_W;
        curr->rect->y = curr->y * DELTA_H;

		TTF_CloseFont(curr->font);
		curr->font = NULL;
        TTF_Font *font = TTF_OpenFont("graphics/DejaVuSans.ttf", curr->size * DELTA);
        curr->font = font;

		SDL_Surface *surface = NULL;

		if (*curr->text) {
			surface = TTF_RenderUTF8_Blended(font, curr->text, curr->color);
            if (!surface) {
                fprintf(stderr, "TTF_RenderUTF8_Blended: %s\n", TTF_GetError());
                return;
            }
			curr->rect->w = surface->w;
		}
		else {
			surface = TTF_RenderUTF8_Blended(font, " ", curr->color);
			if (!surface) {
			    fprintf(stderr, "TTF_RenderUTF8_Blended: %s\n", TTF_GetError());
			    return;
			}
			curr->rect->w = 0;
		}

        curr->rect->h = surface->h;

		SDL_DestroyTexture(curr->texture);
		SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
		curr->texture = texture;
		
		SDL_FreeSurface(surface);	

        curr = curr->next;
    }
}
void update_texts(Text *texts);

void resize_buttons(SDL_Renderer *renderer, Button *buttons) {
    Button *curr = buttons;

    while (curr) {
		resize_rectangles(curr->rect);
		resize_texts(renderer, curr->text);

		center_text(curr->text, curr->rect);

		curr = curr->next;
	}
}
void update_button(Button *button, Frame *frame) {
	int hovered = 0;

	SDL_Color c = button->rect->color;

	if (button->interactible) {
		hovered = 1;
	}

	SDL_SetRenderDrawColor(
		Renderer,
		(hovered == 1) ? (c.r > 100 ? c.r - 100 : 0) : c.r,
		(hovered == 1) ? (c.g > 100 ? c.g - 100 : 0) : c.g,
		(hovered == 1) ? (c.b > 100 ? c.b - 100 : 0) : c.b,
		c.a
	);

	if (hovered) {
		mouse_hover = 1;
		SDL_RenderFillRect(Renderer, button->rect->rect);
		display_texts(button->text);
		display_frame(frame->subframe);
		return;
	}
}

void resize_text_areas(SDL_Renderer *renderer, TextArea *text_areas) {
    TextArea *curr = text_areas;

    while (curr) {
        resize_rectangles(curr->rect);
        resize_texts(renderer, curr->text);

        lower_text(curr->text, curr->rect);

        curr = curr->next;
    }
}

void update_text_areas(TextArea *text_areas) {
	mouse_hover = 2;	
}

void resize_cursors(SDL_Renderer *renderer, Cursor *cursors) {
	Cursor *curr = cursors;

	while (curr) {
		resize_rectangles(curr->background);
        resize_rectangles(curr->loading);
        resize_rectangles(curr->dragzone);

        resize_texts(renderer, curr->min_text);
        resize_texts(renderer, curr->max_text);
        resize_texts(renderer, curr->val_text);

		curr->val_text->y -= curr->val_text->rect->h / DELTA_H;
	    curr->val_text->rect->y -= curr->val_text->rect->h;

		curr = curr->next;
	}
}

void update_cursors(Cursor *cursor, Frame *frame) {
	if (!cursor) return;
    int hovered = 1;

    SDL_Color c = cursor->dragzone->color;

    SDL_SetRenderDrawColor(
        Renderer,
        (hovered == 1) ? (c.r > 100 ? c.r - 100 : 0) : c.r,
        (hovered == 1) ? (c.g > 100 ? c.g - 100 : 0) : c.g,
        (hovered == 1) ? (c.b > 100 ? c.b - 100 : 0) : c.b,
        c.a
    );

    if (hovered) {
        mouse_hover = 1;
        SDL_RenderFillRect(Renderer, cursor->dragzone->rect);
		
		display_frame(frame->subframe);
    }

	if (!left_pressed) return;

    int x, y;
	SDL_GetMouseState(&x, &y);

	modify_pos_cursor(x, y, cursor);
	float new_val = get_value_cursor(cursor);
	cursor->value = new_val;

    char str[100];
    sprintf(str, "%.2f", new_val);
	modify_text(Renderer, cursor->val_text, str);
}

void resize_image();
void update_image();

void resize_all(Frame *frame) {
	resize_rectangles(frame->rectangles);
	resize_texts(Renderer, frame->texts);
	resize_buttons(Renderer, frame->buttons);
	resize_text_areas(Renderer, frame->text_areas);
	resize_cursors(Renderer, frame->cursors);
	printf("Resized\n");
}

void update_all(Frame *frame) {
	if (!frame) return;	

	int old_w = WIN_W;
	int old_h = WIN_H;
	SDL_GetWindowSize(Window, &WIN_W, &WIN_H);
	
	WIN_MIN = WIN_W > WIN_H ? WIN_H : WIN_W;

	float ratio_w;
	float ratio_h;
	float ratio;
	ratio_w = WIN_W / (float)WINDOW_DEFAULT_W;
	ratio_h = WIN_H / (float)WINDOW_DEFAULT_H;
	ratio = ratio_w > ratio_h ? ratio_h : ratio_w;

	DELTA_W = WINDOW_DEFAULT_W * ratio;
	DELTA_H = WINDOW_DEFAULT_H * ratio;
	DELTA = DELTA_W > DELTA_H ? DELTA_H : DELTA_W;

	if (old_w != WIN_W || old_h != WIN_H) {
		Frame *curr_resize = frame;

		while (curr_resize) {
			resize_all(curr_resize);
			curr_resize = curr_resize->subframe;
		}
	}

    int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);

	Frame *curr = frame;

	while (curr->subframe) {
		curr = curr->subframe;
	}

	while (curr) {
		Button *btn_hover = collision_buttons(mouseX, mouseY, curr->buttons);		
		if (btn_hover) {
			update_button(btn_hover, frame);
			break;
		}

		TextArea *text_area_hover = collision_text_areas(mouseX, mouseY, curr->text_areas);
		if (text_area_hover) {
			update_text_areas(text_area_hover);
			break;
		}

		Cursor *cursor_hover = collision_cursors(mouseX, mouseY, curr->cursors);
		if (cursor_hover && collision_rect(mouseX, mouseY, *cursor_hover->dragzone->rect)) {
			update_cursors(cursor_hover, frame);
			break;
		}

		if (curr == frame)
			break;

		curr = curr->parentframe;
	}
	
	update_cursors(frame->current_cursor, frame);

	update_all(frame->subframe);
}
