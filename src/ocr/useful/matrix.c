#include "ocr/useful/matrix.h"

#include <stdio.h>
#include <stdlib.h>

int *coordinates_to_matrix(Image *img, int x0, int y0, int x1, int y1)
{
	if (!img || !img->pixels || x0 < 0 || y0 < 0 ||
		x1 < x0 || y1 < y0 || img->width <= x1 || img->height <= y1)
		return NULL;

	int *out = malloc(sizeof(int) * (x1 - x0 + 1) * (y1 - y0 + 1));

	int w = x1 - x0 + 1;
	int h = y1 - y0 + 1;

	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			out[y * w + x] = img->pixels[(y0 + y) * img->width + (x0 + x)];
		}
	}

	return out;
}

int *matrix_to_28x28(int *matrix, int w, int h)
{
    if (!matrix || w <= 0 || h <= 0)
        return NULL;

    int *out = malloc(28 * 28 * sizeof(int));
    if (!out)
        return NULL;

    for (int i = 0; i < 28 * 28; i++)
        out[i] = 0;

    float scale = (28.0f / w < 28.0f / h) ? 28.0f / w : 28.0f / h;

    int new_w = (int)(w * scale + 0.5f);
    int new_h = (int)(h * scale + 0.5f);

    int offset_x = (28 - new_w) / 2;
    int offset_y = (28 - new_h) / 2;

    for (int y = 0; y < new_h; y++)
    {
        int src_y = (int)(y / scale);

        for (int x = 0; x < new_w; x++)
        {
            int src_x = (int)(x / scale);

            if (src_x >= w) src_x = w - 1;
            if (src_y >= h) src_y = h - 1;

            out[(y + offset_y) * 28 + (x + offset_x)] =
                matrix[src_y * w + src_x];
        }
    }

    return out;
}

uint8_t **expand_matrix(uint8_t **matrix, int w, int h, int new_w, int new_h)
{
	if (!matrix || w <= 0 || h <= 0 || new_w < w || new_h < h)
		return matrix;

	if (new_w == w && new_h == h)
		return matrix;

    uint8_t **new_matrix = realloc(matrix, new_h * sizeof(uint8_t *));

    matrix = new_matrix;

    for (int y = 0; y < h; y++)
    {
		matrix[y] = realloc(matrix[y], new_w * sizeof(uint8_t));

        for (int x = w; x < new_w; x++)
            matrix[y][x] = 0;
    }

    for (int y = h; y < new_h; y++)
    {
        matrix[y] = calloc(new_w, sizeof(uint8_t));
    }

    return matrix;
}

void shift_matrix(uint8_t **matrix, int w, int h, int shift_x, int shift_y)
{
    if (matrix == NULL || w <= 0 || h <= 0)
        return;

    uint8_t **tmp = malloc(h * sizeof(uint8_t *));

    for (int y = 0; y < h; y++)
    {
        tmp[y] = calloc(w, sizeof(uint8_t));
	}

    for (int y = 0; y < h; y++)
    {
	   for (int x = 0; x < w; x++)
        {
            int new_x = x + shift_x;
            int new_y = y + shift_y;

            if (new_x >= 0 && new_x < w && new_y >= 0 && new_y < h)
            {
                tmp[new_y][new_x] = matrix[y][x];
            }
        }
    }

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
            matrix[y][x] = tmp[y][x];

        free(tmp[y]);
    }

    free(tmp);
}

void print_matrix(int *matrix, int w, int h)
{
    if (!matrix || w <= 0 || h <= 0) return;

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
			char symbol = (matrix[y * w + x] == 1) ? '#' : '.';
			printf("%c", symbol);
		}
		printf("\n");
    }
}

void print_matrix_values(int *matrix, int w, int h)
{
    if (!matrix || w <= 0 || h <= 0)
        return;

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
            printf("%d", matrix[y * w + x]);
        printf("\n");
    }
}
