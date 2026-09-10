#include "text.h"
#include "../globals.h"

#include <SDL2/SDL_ttf.h>
#include <string.h>

static SDL_Renderer *g_renderer = NULL;
static TTF_Font *g_font = NULL;

int font_init(SDL_Renderer *renderer, const char *font_file)
{
    g_renderer = renderer;

    g_font = TTF_OpenFont(font_file, 24);

    return g_font != NULL;
}

Text *create_text(char *text, float x, float y, float size, SDL_Color color)
{
	Text *res = malloc(sizeof(Text));

	if (!text)
		text = "\0";

	res->text = strdup(text);
	res->color = color;
	res->x = x;
	res->y = y;
	res->size = size;
    res->len_max = strlen(text);
    if (res->len_max == 0)
        res->len_max = 1;
	res->default_len = res->len_max;

	TTF_Font *font = TTF_OpenFont("graphics/DejaVuSans.ttf", size * DELTA);
	res->font = font;

	SDL_Surface *surface_temp = TTF_RenderUTF8_Blended(font, ".", color);

	if (surface_temp->w)
		res->size_one_element = surface_temp->w / (float)DELTA_W;
	else
		res->size_one_element = 1;

	SDL_FreeSurface(surface_temp);

	SDL_Surface *surface;

	if (*text)
		surface = TTF_RenderUTF8_Blended(font, text, color);
	else
		surface = TTF_RenderUTF8_Blended(font, ".", color);

	if (!surface)
	{
		fprintf(stderr, "TTF_RenderUTF8_Blended: %s\n", TTF_GetError());

		TTF_CloseFont(font);
		free(res->text);
		free(res);

		return NULL;
	}

    SDL_Texture *texture = SDL_CreateTextureFromSurface(g_renderer, surface);
	res->texture = texture;	

	SDL_Rect *rect = malloc(sizeof(SDL_Rect));
    rect->x = x * DELTA_W;
    rect->y = y * DELTA_H;
	rect->w = (text[0] == '\0') ? 0 : surface->w;
	rect->h = surface->h;

	res->rect = rect;

	SDL_FreeSurface(surface);

	res->next = NULL;

	return res;
}

Text *copy_text(Text *text)
{
    if (!text)
        return NULL;

    return create_text(text->text, text->x, text->y, text->size, text->color);
}

Text *add_text(Text *texts, Text *text)
{
	if (!texts) return text;

	Text *curr = texts;

	while (curr->next) {
		curr = curr->next;
	}

	curr->next = text;

	return texts;
}

void display_texts(Text *texts)
{
    Text *curr = texts;

    while (curr)
    {
        if (curr->texture)
            SDL_RenderCopy(g_renderer, curr->texture, NULL, curr->rect);

        curr = curr->next;
    }
}

void destroy_texts(Text *texts)
{
    Text *curr = texts;

    while (curr) {
		Text *temp = curr;
		
		free(curr->text);
		free(curr->rect);
		TTF_CloseFont(curr->font);
		SDL_DestroyTexture(curr->texture);
		
		curr = curr->next;
		free(temp);
	}
}

void destroy_font() {
    if (g_font)
        TTF_CloseFont(g_font);
}

void modify_text(SDL_Renderer *renderer, Text *text, char *content) 
{
    if (!content || !text || !renderer) return;

    SDL_Surface* surface = NULL;
    SDL_Texture* newTexture = NULL;

    if (*content)
	{
        surface = TTF_RenderUTF8_Blended(text->font, content, text->color);
		newTexture = SDL_CreateTextureFromSurface(renderer, surface);
    }

	int w = 0;
	if (surface)
		w = surface->w;

    if (strlen(content) > text->len_max) 
	{
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(newTexture);
    }
    else 
	{
            free(text->text);
			SDL_DestroyTexture(text->texture);

            text->text = strdup(content);
			text->texture = newTexture;

	        if (surface)
				SDL_FreeSurface(surface);

			text->rect->w = w;
    }
}

void add_char(SDL_Renderer *renderer, Text *text, char c) {
    int len = strlen(text->text);
    char buff[len + 2];

    strcpy(buff, text->text);
    buff[len] = c;
    buff[len + 1] = '\0';

    modify_text(renderer, text, buff);
}

void delete_char(SDL_Renderer *renderer, Text *text) {
    int len = strlen(text->text);
    if (!len) return;
	
	char buff[len];

	strncpy(buff, text->text, len - 1);
	buff[len - 1] = '\0';

	modify_text(renderer, text, buff);
}

void set_text_int(SDL_Renderer *renderer, Text *text, int value)
{
    if (!renderer || !text)
        return;

    char buff[text->default_len + 32];

    memcpy(buff, text->text, text->default_len);
    snprintf(buff + text->default_len, sizeof(buff) - text->default_len, "%d", value);

    modify_text(renderer, text, buff);
}

void set_text_double(SDL_Renderer *renderer, Text *text, double value)
{
    if (!renderer || !text)
        return;

    char buff[text->default_len + 64];

    memcpy(buff, text->text, text->default_len);
    snprintf(buff + text->default_len, sizeof(buff) - text->default_len, "%f", value);

    modify_text(renderer, text, buff);
}

