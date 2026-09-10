#include "transform.h"
#include "../../graphics/globals.h"

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <SDL2/SDL_ttf.h>

void rotate_image(Image *src, double angle)
{
    double rad = angle * M_PI / 180.0;

    double c = cos(rad);
    double s = sin(rad);

    int new_width =
        (int)(fabs(src->width * c) + fabs(src->height * s) + 0.5);

    int new_height =
        (int)(fabs(src->width * s) + fabs(src->height * c) + 0.5);

    uint8_t *new_pixels = malloc(new_width * new_height);
    Pixel *new_raw_pixels = malloc(sizeof(Pixel) * new_width * new_height);

    if (!new_pixels || !new_raw_pixels)
    {
        free(new_pixels);
        free(new_raw_pixels);
        return;
    }

    for (int i = 0; i < new_width * new_height; i++)
    {
        new_pixels[i] = 0;
        new_raw_pixels[i] = (Pixel){255, 255, 255, 255};
    }

    double cx_src = src->width / 2.0;
    double cy_src = src->height / 2.0;

    double cx_dst = new_width / 2.0;
    double cy_dst = new_height / 2.0;

    for (int y = 0; y < new_height; y++)
    {
        for (int x = 0; x < new_width; x++)
        {
            double dx = x - cx_dst;
            double dy = y - cy_dst;

            double sx =  c * dx + s * dy + cx_src;
            double sy = -s * dx + c * dy + cy_src;

            int ix = (int)(sx + 0.5);
            int iy = (int)(sy + 0.5);

            if (ix >= 0 && iy >= 0 &&
                ix < src->width && iy < src->height)
            {
                new_pixels[y * new_width + x] =
                    get_pixel(src, ix, iy);

                if (src->raw_pixels)
                    new_raw_pixels[y * new_width + x] =
                        src->raw_pixels[iy * src->width + ix];
            }
        }
    }

    free(src->pixels);
    free(src->raw_pixels);

    src->pixels = new_pixels;
    src->raw_pixels = new_raw_pixels;
    src->width = new_width;
    src->height = new_height;
}

Image *crop_white(Image *src)
{
    int left = src->width;
    int right = -1;
    int top = src->height;
    int bottom = -1;

    for (int y = 0; y < src->height; y++)
    {
        for (int x = 0; x < src->width; x++)
        {
            if (get_pixel(src, x, y))
            {
                if (x < left)
                    left = x;

                if (x > right)
                    right = x;

                if (y < top)
                    top = y;

                if (y > bottom)
                    bottom = y;
            }
        }
    }

    if (right == -1)
        return NULL;

    Image *dst = malloc(sizeof(Image));

    if (dst == NULL)
        return NULL;

    dst->width = right - left + 1;
    dst->height = bottom - top + 1;

    dst->pixels = malloc(dst->width * dst->height);

    if (dst->pixels == NULL)
    {
        free(dst);
        return NULL;
    }

    for (int y = 0; y < dst->height; y++)
    {
        for (int x = 0; x < dst->width; x++)
        {
            set_pixel(dst, x, y,
						get_pixel(src, x + left, y + top));
        }
    }

    return dst;
}

static double hough_angle_range(Image *img, double start_angle, double end_angle,
                                double precision, int use_perpendicular)
{
    int width = img->width;
    int height = img->height;

    int rho_max = (int)ceil(sqrt(width * width + height * height)); // rho is the maximum distance from the origin

    double rho_step = 0.5; // let 0.5 for normal images, put precision or even less for very thin lines / very small images
    int nb_rho = ceil(2*rho_max/rho_step)+1;

    int nb_angles = (int)ceil((end_angle - start_angle) / precision) + 1;

    int *acc = calloc(nb_angles * nb_rho, sizeof(int)); // the accumulator contains every possibilities of rho and angle

    double *cos_table = malloc(nb_angles * sizeof(double)); // pre compute them for better efficiency
    double *sin_table = malloc(nb_angles * sizeof(double));

    for (int t = 0; t < nb_angles; t++)
    {
        double angle = start_angle + t * precision;
        double rad = angle * M_PI / 180.0; // convert angle to rad

        cos_table[t] = cos(rad);
        sin_table[t] = sin(rad);
    }

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (img->pixels[y * width + x] == 0)
                continue;

            for (int t = 0; t < nb_angles; t++) // vote on a pixel with every combinations
            {
                double rho = x * cos_table[t] + y * sin_table[t];
                int index = lround((rho + rho_max) / rho_step);

                if (index >= 0 && index < nb_rho)
                    acc[t * nb_rho + index]++;
            }
        }
    }

    int *score = calloc(nb_angles, sizeof(int));

    for (int t = 0; t < nb_angles; t++) // find the strongest line for every angle
    {
        int max = 0;

        for (int r = 0; r < nb_rho; r++)
        {
            int votes = acc[t * nb_rho + r];

            if (votes > max)
                max = votes;
        }

        score[t] = max;
    }

    int best_theta = 0;
    int best_score = 0;

    if (use_perpendicular) // useful if "+" shapes
    {
        int offset90 = (int)lround(90.0 / precision);

        for (int t = 0; t < nb_angles; t++) // combine horizontal and vertical lines
        {
            int t90 = (t + offset90) % nb_angles;

            int s = score[t] + score[t90];

            if (s > best_score)
            {
                best_score = s;
                best_theta = t;
            }
        }
    }
    else
    {
        for (int t = 0; t < nb_angles; t++) // only keep the strongest line
        {
            if (score[t] > best_score)
            {
                best_score = score[t];
                best_theta = t;
            }
        }
    }

    free(acc);
    free(score);
    free(cos_table);
    free(sin_table);

    return start_angle + best_theta * precision;
}

double hough_angle(Image *img, double precision)
{
    if (precision <= 0.0)
    {
        printf("Error: hough_angle, precision <= 0\n");
        return 0.0;
    }

    double coarse = hough_angle_range(img, 0.0, 180.0, 1.0, 0);

    double start = coarse - 2.0;
    double end = coarse + 2.0;

    if (start < 0.0)
        start = 0.0;

    if (end > 180.0)
        end = 180.0;

    double best = hough_angle_range(img, start, end, precision, 0);

    best -= 90.0;

    if (best < -90.0)
        best += 180.0;

    if (best > 90.0)
        best -= 180.0;

    return best;
}

void modify_sudoku_image(Image *img, int *empty_sudoku, int *solved_sudoku)
{
	if (!img)
	    return;

	if (!img->squares_coordinates)
	{
	    printf("No squares coordinates\n");
	    return;
	}

	SDL_Texture *texture = image_to_texture(Renderer, img);

	int size = (img->squares_coordinates[3] - img->squares_coordinates[1]);

	TTF_Font *font = TTF_OpenFont("graphics/DejaVuSans.ttf", size);

	uint8_t r, g, b, a;

	for (int i = 0; i < 81; i++)
	{
		if (empty_sudoku[i] != 0)
			continue;

		char buff[2];
		buff[0] = solved_sudoku[i] + '0';
		buff[1] = '\0';

		SDL_Surface *surface = TTF_RenderUTF8_Blended(font, buff, Black);

		Uint32 *pixels = (Uint32 *)surface->pixels;

		int start_x = img->squares_coordinates[4 * i + 0];
		int start_y = img->squares_coordinates[4 * i + 1];

		for (int y = 0; y < surface->h; y++)
		{
			for (int x = 0; x < surface->w; x++)
			{
				Uint32 *row = (Uint32 *)((Uint8 *)surface->pixels + y * surface->pitch);
				Uint32 pixel = row[x];
				SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);

				if (a == 0)
					continue;

				img->raw_pixels[(start_y + y) * img->width + start_x + x].r = r;
                img->raw_pixels[(start_y + y) * img->width + start_x + x].g = g;
                img->raw_pixels[(start_y + y) * img->width + start_x + x].b = b;
				img->raw_pixels[(start_y + y) * img->width + start_x + x].a = a;
			}
		}

		SDL_FreeSurface(surface);	
	}

	TTF_CloseFont(font);
}
