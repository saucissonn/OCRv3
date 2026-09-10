#ifndef TEXT_H
#define TEXT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct Text {
	char *text;
	float x;
	float y;
	float size;
	float size_one_element;
	int len_max;
	int default_len;

	SDL_Color color;
	TTF_Font *font;
	SDL_Rect *rect;
	SDL_Texture *texture;

	struct Text *next;
} Text;

int font_init(SDL_Renderer *renderer, const char *font_file);

Text *create_text(char *text, float x, float y, float size, SDL_Color color);
Text *copy_text(Text *text);

Text *add_text(Text *texts, Text *text);

void display_texts(Text *texts);
void destroy_texts(Text *texts);
void destroy_font();

void modify_text(SDL_Renderer *renderer, Text *text, char *content);
void add_char(SDL_Renderer *renderer, Text *text, char c);
void delete_char(SDL_Renderer *renderer, Text *text);

void set_text_int(SDL_Renderer *renderer, Text *text, int value);
void set_text_double(SDL_Renderer *renderer, Text *text, double value);

#endif
