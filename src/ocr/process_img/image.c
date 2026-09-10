#include "image.h"
#include "detection.h"

#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

Image *load_png(const char *filename)
{
    SDL_Surface *src = IMG_Load(filename);

    if (src == NULL)
    {
        SDL_Log("%s", IMG_GetError());
        return NULL;
    }

    SDL_Surface *surface = SDL_ConvertSurfaceFormat(src, SDL_PIXELFORMAT_RGBA32, 0);

    SDL_FreeSurface(src);

    Image *img = malloc(sizeof(Image));

    img->width = surface->w;
    img->height = surface->h;

    img->pixels = malloc(img->width * img->height);
	img->raw_pixels = malloc(sizeof(Pixel) * img->width * img->height);

    SDL_LockSurface(surface);

    Uint8 *pixels = (Uint8 *)surface->pixels;

    for (int y = 0; y < img->height; y++)
    {
        for (int x = 0; x < img->width; x++)
        {
            Uint8 *p = pixels + y * surface->pitch + x * 4;

            int gray = (p[0] + p[1] + p[2]) / 3;

			long int idx = y * img->width + x;

            img->raw_pixels[idx].r = p[0];
            img->raw_pixels[idx].g = p[1];
            img->raw_pixels[idx].b = p[2];
            img->raw_pixels[idx].a = p[3];

            img->pixels[idx] = (gray < 220) ? 1 : 0;
        }
    }

    SDL_UnlockSurface(surface);

    SDL_FreeSurface(surface);

	img->squares = NULL;
	img->valid_squares = NULL;
	img->squares_coordinates = NULL;

    return img;
}

void process_image(Image *img)
{
	img->squares = get_squares_sudoku(img);
	img->valid_squares = get_valid_squares(img->squares);
	img->squares_coordinates = detect_squares(img, 0.3);
}

void free_image(Image *img)
{
    if (img == NULL)
        return;

    free(img->pixels);
	free(img->raw_pixels);
	free(img->squares_coordinates);

	if (img->squares)
	{
		for (int i = 0; i < 81; i++)
			free(img->squares[i]);
	}

	free(img->squares);
	free(img->valid_squares);
    free(img);
}

uint8_t get_pixel(Image *img, int x, int y)
{
    return img->pixels[y * img->width + x];
}

void set_pixel(Image *img,
               int x,
               int y,
               uint8_t value)
{
    img->pixels[y * img->width + x] = value;
}

SDL_Texture *image_to_texture(SDL_Renderer *renderer, Image *img)
{
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(
        0,
        img->width,
        img->height,
        32,
        SDL_PIXELFORMAT_RGBA8888);

    if (!surface)
        return NULL;

    Uint32 white = SDL_MapRGBA(surface->format, 255, 255, 255, 255);

    Uint32 *pixels = surface->pixels;

    for (int y = 0; y < img->height; y++)
    {
        for (int x = 0; x < img->width; x++)
		{
			uint8_t r = img->raw_pixels[y * img->width + x].r;
            uint8_t g = img->raw_pixels[y * img->width + x].g;
            uint8_t b = img->raw_pixels[y * img->width + x].b;
            uint8_t a = img->raw_pixels[y * img->width + x].a;

            pixels[y * img->width + x] = SDL_MapRGBA(surface->format, r, g, b, a);
		}
	}

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FreeSurface(surface);

    return texture;
}
