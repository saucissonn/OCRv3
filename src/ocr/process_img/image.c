#include "ocr/process_img/image.h"
#include "ocr/process_img/detection.h"
#include "ocr/process_img/transform.h"
#include "ocr/useful/matrix.h"

#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

void binarize_threshold_from_colors(Image *img, SDL_Surface *surface, int th)
{
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

			img->pixels[idx] = (gray < th) ? 1 : 0;
		}
	}

	SDL_UnlockSurface(surface);
}

void binarize_threshold(Image *img, int th)
{
    for (int y = 0; y < img->height; y++)
    {
        for (int x = 0; x < img->width; x++)
        {
			long int idx = y * img->width + x;

            uint8_t r = img->raw_pixels[idx].r;
            uint8_t g = img->raw_pixels[idx].g;
            uint8_t b = img->raw_pixels[idx].b;

			int gray = (r + g + b) / 3;

            img->pixels[idx] = (gray < th) ? 1 : 0;
        }
    }
}

void binarize_threshold_mask(Image *img, uint8_t *mask, int th)
{
    for (int y = 0; y < img->height; y++)
    {
        for (int x = 0; x < img->width; x++)
        {
            long int idx = y * img->width + x;

			if (mask[idx] == 0)
			{
				uint8_t r = img->raw_pixels[idx].r;
				uint8_t g = img->raw_pixels[idx].g;
				uint8_t b = img->raw_pixels[idx].b;

				int gray = (r + g + b) / 3;

				img->pixels[idx] = (gray < th) ? 1 : 0;
			}
			else
			{
				img->pixels[idx] = 0;
			}
	    }
    }
}


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

	// Import with high threshold
	binarize_threshold_from_colors(img, surface, 220);

	double h_angle = hough_angle(img, 0.1);
	printf("angle: %lf\n", h_angle);
	rotate_image(img, -h_angle);

	binarize_threshold(img, 180);	

	SDL_FreeSurface(surface);

	img->nb_rectangle = 0;
	img->nb_puzzle_rectangle = 0;
	img->nb_word_rectangle = 0;

	img->rectangles = NULL;
	img->puzzle_rectangles = NULL;
	img->word_rectangles = NULL;

	img->px0 = -1;
	img->py0 = -1;
	img->px1 = -1;
	img->py1 = -1;

	return img;
}

void process_image(Image *img)
{
	img->puzzle_rectangles = get_puzzle_rectangles(img);
	img->word_rectangles = get_word_rectangles(img);
}

void free_image(Image *img)
{
	if (img == NULL)
		return;

	free(img->pixels);
	free(img->raw_pixels);

	if (img->rectangles)
	{
        for (int i = 0; i < img->nb_rectangle; i++)
        {
            destroy_image_rect(img->rectangles[i]);
        }
	}
	free(img->rectangles);

	if (img->puzzle_rectangles)
	{
        for (int i = 0; i < img->nb_puzzle_rectangle; i++)
        {
            destroy_image_rect(img->puzzle_rectangles[i]);
        }
	}
	free(img->puzzle_rectangles);

	if (img->word_rectangles)
	{
        for (int i = 0; i < img->nb_word_rectangle; i++)
        {
            destroy_image_rect_group(img->word_rectangles[i]);
        }
	}
	free(img->word_rectangles);

	free(img);
	img = NULL;
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
			// Normal colors
			uint8_t r = img->raw_pixels[y * img->width + x].r;
			uint8_t g = img->raw_pixels[y * img->width + x].g;
			uint8_t b = img->raw_pixels[y * img->width + x].b;
			uint8_t a = img->raw_pixels[y * img->width + x].a;

			// Debug colors
			uint8_t rd = 0;
			uint8_t gd = 0;
			uint8_t bd = 0;

			if (img->pixels[y * img->width + x] == 1)
			{
				rd = 255;
				gd = 255;
				bd = 255;
			}
			else if (img->pixels[y * img->width + x] == 2) // Debug color puzzle
			{
				rd = 255;
			}
            else if (img->pixels[y * img->width + x] == 3) // Debug color word
            {
                bd = 255;
            }

			pixels[y * img->width + x] = SDL_MapRGBA(surface->format, rd, gd, bd, a);
		}
	}

	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_FreeSurface(surface);

	return texture;
}

ImageRect *create_image_rect(Image *img, int x0, int y0, int x1, int y1)
{
	if (!img)
	{
		printf("Error, create_image_rect, no image found\n");
		return NULL;
	}

	if (x0 < 0 || y0 < 0 || x0 >= img->width || 0 >= img->height)
	{
		printf("Error, create_image_rect, invalid coordinates\n");
		return NULL;
	}

	if (x1 < x0 || y1 < y0 || x1 >= img->width || y1 >= img->height)
	{
		printf("Error, create_image_rect, invalid coordinates\n");
		return NULL;
	}

	ImageRect *res = malloc(sizeof(ImageRect));

	res->x0 = x0;
	res->y0 = y0;
	res->x1 = x1;
	res->y1 = y1;

	res->w = x1 - x0 + 1;
	res->h = y1 - y0 + 1;

	uint8_t **pixels = malloc(sizeof(uint8_t *) * res->h);

	for (int y = 0; y < res->h; y++)
	{
		pixels[y] = calloc(res->w, sizeof(uint8_t));
		for (int x = 0; x < res->w; x++)
		{
			pixels[y][x] = img->pixels[(y + y0) * img->width + x + x0];
		}
	}

	res->pixels = pixels;

	return res;
}

void destroy_image_rect(ImageRect *rect)
{
	if (!rect) return;

	if (!rect->pixels)
	{
		free(rect);
		return;
	}

	for (int y = 0; y < rect->h; y++)
	{
		free(rect->pixels[y]);
	}

	free(rect->pixels);
	free(rect);
}

int add_point_image_rect(ImageRect *rect, Image *img, int x, int y)
{
	if (!img)
	{
		printf("Error, add_point_image_rect, no image found\n");
		return 1;
	}

	if (!rect || !rect->pixels)
	{
		printf("Error, add_point_image_rect, no rect found\n");
		return 1;
	}

	if (x < 0 || y < 0 || x >= img->width || y >= img->height)
	{
		printf("Error, add_point_image_rect, invalid coordinates\n");
		return 1;
	}

	int old_x0 = rect->x0;
	int old_y0 = rect->y0;
	int old_x1 = rect->x1;
	int old_y1 = rect->y1;

	int new_x0 = old_x0;
	int new_y0 = old_y0;
	int new_x1 = old_x1;
	int new_y1 = old_y1;

	if (x < new_x0)
		new_x0 = x;

	if (y < new_y0)
		new_y0 = y;

	if (x > new_x1)
		new_x1 = x;

	if (y > new_y1)
		new_y1 = y;

	int new_w = new_x1 - new_x0 + 1;
	int new_h = new_y1 - new_y0 + 1;

	int shift_x = old_x0 - new_x0;
	int shift_y = old_y0 - new_y0;

	if (new_w != rect->w || new_h != rect->h)
	{
		rect->pixels = expand_matrix(rect->pixels, rect->w, rect->h, new_w, new_h);

		if (!rect->pixels)
			return 1;

		if (shift_x != 0 || shift_y != 0)
		{
			shift_matrix(rect->pixels, new_w, new_h, shift_x, shift_y);
		}

		rect->w = new_w;
		rect->h = new_h;

		rect->x0 = new_x0;
		rect->y0 = new_y0;
		rect->x1 = new_x1;
		rect->y1 = new_y1;
	}

	rect->pixels[y - rect->y0][x - rect->x0] = 1;

	return 0;
}

int collision_image_rect(ImageRect *rect, int x, int y) // 1 collision, 0 no collision or error
{
	if (!rect || !rect->pixels)
	{
		printf("Error, collision_image_rect, no rect found\n");
		return 0;
	}

	if (x < 0 || y < 0)
	{
		printf("Error, collision_image_rect, invalid coordinates\n");
		return 0;
	}

	if (x >= rect->x0 && y >= rect->y0 && x <= rect->x1 && y <= rect->y1)
	{
		return 1;
	}

	return 0;
}

void print_image_rect(ImageRect *rect)
{
	if (rect == NULL || rect->pixels == NULL) {
		printf("Error: print_image_rect, NULL pointer\n");
		return;
	}

	printf("ImageRect: (%d, %d) -> (%d, %d), size = %dx%d\n",
			rect->x0, rect->y0,
			rect->x1, rect->y1,
			rect->w, rect->h);

	for (int y = 0; y < rect->h; y++)
	{
		for (int x = 0; x < rect->w; x++)
		{
			char c = (rect->pixels[y][x] == 1) ? '#' : '.';
			printf("%c", c);
		}
		printf("\n");
	}
}

double ImageRectDistance(ImageRect *a, ImageRect *b)
{
	int dx = 0;
	int dy = 0;

	if (a->x1 < b->x0)
		dx = b->x0 - a->x1;
	else if (b->x1 < a->x0)
		dx = a->x0 - b->x1;

	if (a->y1 < b->y0)
		dy = b->y0 - a->y1;
	else if (b->y1 < a->y0)
		dy = a->y0 - b->y1;

	return sqrt((double)dx * dx + (double)dy * dy);
}

void bounding_box_image_rects(ImageRect **rects, int nb, int *x0, int *y0, int *x1, int *y1)
{
    if (!rects || nb == 0)
	{
        printf("Error: bounding_box_image_rects, invalid args\n");
        return;
    }

	int x_min = rects[0]->x0;
    int y_min = rects[0]->y0;
    int x_max = rects[0]->x1;
    int y_max = rects[0]->y1;	

	for (int i = 1; i < nb; i++)
	{
		int x0_ = rects[i]->x0;
		int y0_ = rects[i]->y0;
		int x1_ = rects[i]->x1;
		int y1_ = rects[i]->y1;

		if (x0_ < x_min)
		{
			x_min = x0_;
		}
        if (y0_ < y_min)
        {
            y_min = y0_;
        }
        if (x1_ > x_max)
        {
            x_max = x1_;
        }
        if (y1_ > y_max)
        {
            y_max = y1_;
        }
	}

	*x0 = x_min;
    *y0 = y_min;
	*x1 = x_max;
	*y1 = y_max;

	printf("puzzle bbox: x0=%d, y0=%d, x1=%d, y1=%d\n", *x0, *y0, *x1, *y1);
}

int find_max_puzzle_rectangles_x(ImageRect **rects, int nb, int y0, int y1)
{
    if (!rects || nb == 0)
    {
        printf("Error: find_max_puzzle_rectangles_x, invalid args\n");
        return -1;
    }

    int maxi = 0;
    for (int y = y0; y < y1; y++)
    {
        int count = 0;

        for (int r = 0; r < nb; r++)
        {
            ImageRect *rect = rects[r];

            if (rect->y0 <= y && y <= rect->y1)
            {
                count += 1;
            }
        }

        if (maxi < count)
        {
            maxi = count;
        }
    }

    return maxi;
}

int find_max_puzzle_rectangles_y(ImageRect **rects, int nb, int x0, int x1)
{
    if (!rects || nb == 0)
    {
        printf("Error: find_max_puzzle_rectangles_y, invalid args\n");
        return -1;
    }

	int maxi = 0;
	for (int x = x0; x < x1; x++)
	{
		int count = 0;

		for (int r = 0; r < nb; r++)
		{
			ImageRect *rect = rects[r];

			if (rect->x0 <= x && x <= rect->x1)
			{
				count += 1;
			}
		}

		if (maxi < count)
		{
			maxi = count;
		}
	}

	return maxi;
}

ImageRectGroup *create_image_rect_group(Image *img, int x0, int y0, int x1, int y1)
{
    if (!img)
    {
        printf("Error, create_image_rect_group, no image found\n");
        return NULL;
    }

    if (x0 < 0 || y0 < 0 || x0 >= img->width || 0 >= img->height)
    {
        printf("Error, create_image_rect_group, invalid coordinates\n");
        return NULL;
    }

    if (x1 < x0 || y1 < y0 || x1 >= img->width || y1 >= img->height)
    {
        printf("Error, create_image_rect_group, invalid coordinates\n");
        return NULL;
    }

    ImageRectGroup *res = malloc(sizeof(ImageRectGroup));

    res->x0 = x0;
    res->y0 = y0;
    res->x1 = x1;
    res->y1 = y1;

    res->w = x1 - x0 + 1;
    res->h = y1 - y0 + 1;

	res->nb_char = 0;	
	res->chars = NULL;

    return res;
}

void destroy_image_rect_group(ImageRectGroup *rect)
{
    if (!rect) return;

    if (!rect->chars)
    {
        free(rect);
        return;
    }

    for (int y = 0; y < rect->nb_char; y++)
    {
        destroy_image_rect(rect->chars[y]);
    }
    free(rect);
}

void add_to_image_rect_group(ImageRectGroup *rect_grp, ImageRect *rect)
{
    if (!rect) return;
    
	rect_grp->nb_char += 1;

	rect_grp->chars = realloc(rect_grp->chars, sizeof(ImageRect *) * rect_grp->nb_char);
	
	rect_grp->chars[rect_grp->nb_char - 1] = rect;
}
