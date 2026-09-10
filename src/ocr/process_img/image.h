#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>
#include <SDL2/SDL.h>

typedef struct {
    uint8_t r, g, b, a;
} Pixel;

typedef struct {
    int width;
    int height;
    uint8_t *pixels;    // 0 = white, 1 = black
	Pixel *raw_pixels;

	int **squares;
	int *valid_squares;
	int *squares_coordinates;
} Image;

Image *load_png(const char *filename);
void process_image(Image *img);
void free_image(Image *img);

uint8_t get_pixel(Image *img, int x, int y);
void set_pixel(Image *img, int x, int y, uint8_t value);

SDL_Texture *image_to_texture(SDL_Renderer *renderer, Image *img);

#endif
