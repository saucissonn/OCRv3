#include "matrix.h"

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

void print_matrix(int *matrix, int w, int h)
{
    if (!matrix || w <= 0 || h <= 0)
        return;

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
