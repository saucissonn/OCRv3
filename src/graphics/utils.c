#include "gui_elements/text.h"
#include "utils.h"
#include "globals.h"
#include "../ocr/useful/globals_ocr.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <unistd.h>

int collision_rect(int x, int y, SDL_Rect rect)
{
    return (x >= rect.x &&
            x < rect.x + rect.w &&
            y >= rect.y &&
            y < rect.y + rect.h);
}

void init_global_variables()
{
    // Define global variables for graphics
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return;
    }

    arrowCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    handCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    textCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        fprintf(stderr, "IMG_Init: %s\n", IMG_GetError());
        return;
    }

    if (TTF_Init() == -1)
    {
        fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
        return;
    }

    Window = SDL_CreateWindow(
        "Drag & Drop PNG",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIN_W,
        WIN_H,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (!Window)
        return;

    SDL_SetWindowMinimumSize(Window, WINDOW_MIN_W, WINDOW_MIN_H);

    Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_SOFTWARE);

    if (!Renderer)
        return;

    if (!font_init(Renderer, "graphics/DejaVuSans.ttf"))
        fprintf(stderr, "Impossible to load the font\n");

	// Define global variables for the OCR
	NB_THREADS_MAX = sysconf(_SC_NPROCESSORS_ONLN) - 2;
	if (NB_THREADS_MAX < 1)
		NB_THREADS_MAX = 1;
}

#include <stdlib.h>

char *itoa_base_n(int x, int n)
{
    char buffer[32];
    int i = 0;
    int negative = 0;
    unsigned int value;

    if (n == 0)
    {
        char *res = malloc(2);
        if (!res)
            return NULL;
        res[0] = '0';
        res[1] = '\0';
        return res;
    }

    if (x < 0)
    {
        negative = 1;
        value = (unsigned int)(-x);
    }
    else
        value = (unsigned int)x;

    while (value > 0)
    {
        buffer[i++] = '0' + (value % n);
        value /= n;
    }

    if (negative)
        buffer[i++] = '-';

    char *res = malloc(i + 1);
    if (!res)
        return NULL;

    res[i] = '\0';
    for (int j = 0; j < i; j++)
        res[j] = buffer[i - 1 - j];

    return res;
}
