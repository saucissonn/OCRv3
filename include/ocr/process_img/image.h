#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>
#include <SDL2/SDL.h>

typedef struct {
    uint8_t r, g, b, a;
} Pixel;

typedef struct {
    int x0;
    int y0;
    int x1;
    int y1;

    int w;
    int h;

    uint8_t **pixels;    // 0 = white, 1 = black
} ImageRect;

typedef struct {
    int width;
    int height;
    uint8_t *pixels;    // 0 = white, 1 = black
	Pixel *raw_pixels;

	int nb_rectangle;
	int nb_puzzle_rectangle;
	int nb_word_rectangle;

	ImageRect **rectangles;
	ImageRect **puzzle_rectangles;
    ImageRect **word_rectangles;
} Image;

void binarize_threshold_mask(Image *img, uint8_t *mask, int th);

Image *load_png(const char *filename);
void process_image(Image *img);
void free_image(Image *img);

uint8_t get_pixel(Image *img, int x, int y);
void set_pixel(Image *img, int x, int y, uint8_t value);

SDL_Texture *image_to_texture(SDL_Renderer *renderer, Image *img);

ImageRect *create_image_rect(Image *img, int x0, int y0, int x1, int y1);
void destroy_image_rect(ImageRect *rect);
int add_point_image_rect(ImageRect *rect, Image *img, int x, int y);
int collision_image_rect(ImageRect *rect, int x, int y); // 1 collision, 0 no collision or error
void print_image_rect(ImageRect *rect);

double ImageRectDistance(ImageRect *a, ImageRect *b);
void bounding_box_image_rects(ImageRect **rects, int nb, int *x0, int *y0, int *x1, int *y1);
int find_max_puzzle_rectangles_x(ImageRect **rects, int nb, int y0, int y1);
int find_max_puzzle_rectangles_y(ImageRect **rects, int nb, int x0, int x1);

#endif
