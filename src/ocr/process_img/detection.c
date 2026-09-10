#include "../process_img/detection.h"
#include "../useful/matrix.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

int *detect_lines(Image *img, float size, int direction) // direction 0 is horizontal and 1 is vertical
{
    if (!img || !img->pixels || size <= 0.0f || size > 1.0f)
        return NULL;

    int min_length = direction == 0 ? (int)(img->width * size) : (int)(img->height * size);

    int capacity = 16;
    int count = 0;
    int *lines = malloc(capacity * 4 * sizeof(int));

    int first = 1;
    int start_x = -1;
    int end_x = -1;
    int start_y = -1;
    int end_y = -1;

    int outer = direction == 0 ? img->height : img->width;
    int inner = direction == 0 ? img->width : img->height;

    for (int i = 0; i < outer; i++)
    {
        int j = 0;

        while (j < inner)
        {
            // skip whites
            while (j < inner)
            {
                int x = direction == 0 ? j : i;
                int y = direction == 0 ? i : j;

                if (img->pixels[y * img->width + x] != 0)
                    break;

                j++;
            }

            if (j >= inner)
                break;

            if (direction == 0)
                start_x = j;
            else
                start_y = j;

            // detect blacks
            while (j < inner)
            {
                int x = direction == 0 ? j : i;
                int y = direction == 0 ? i : j;

                int black = img->pixels[y * img->width + x] == 1;

                if (!black)
                {
                    int nx = direction == 0 ? j : i + 1;
                    int ny = direction == 0 ? i + 1 : j;

                    if ((direction == 0 && i + 1 < img->height) || (direction == 1 && i + 1 < img->width))
                        black = img->pixels[ny * img->width + nx] == 1;
                }

                if (!black)
                    break;

                j++;
            }

            if (direction == 0)
                end_x = j - 1;
            else
                end_y = j - 1;

            int length = direction == 0 ? end_x - start_x + 1 : end_y - start_y + 1;

            if (length >= min_length)
            {
                if (count == capacity)
                {
                    capacity *= 2;
                    int *tmp = realloc(lines, capacity * 4 * sizeof(int));
                    lines = tmp;
                }

                if (first)
                {
                    if (direction == 0)
                    {
                        start_y = i;
                        end_y = i;
                    }
                    else
                    {
                        start_x = i;
                        end_x = i;
                    }

                    first = 0;
                }

                if ((direction == 0 && i <= end_y + 1) || (direction == 1 && i <= end_x + 1))
                {
                    if (direction == 0)
                        end_y = i;
                    else
                        end_x = i;
                }
                else
                {
                    lines[count * 4 + 0] = start_x;
                    lines[count * 4 + 1] = start_y;
                    lines[count * 4 + 2] = end_x;
                    lines[count * 4 + 3] = end_y;

                    if (direction == 0)
                    {
                        start_y = i;
                        end_y = i;
                    }
                    else
                    {
                        start_x = i;
                        end_x = i;
                    }

                    count++;
                }
            }
        }
    }

    lines = realloc(lines, (count * 4 + 8) * sizeof(int));

    lines[count * 4 + 0] = start_x;
    lines[count * 4 + 1] = start_y;
    lines[count * 4 + 2] = end_x;
    lines[count * 4 + 3] = end_y;
    lines[count * 4 + 4] = -1;
    lines[count * 4 + 5] = -1;
    lines[count * 4 + 6] = -1;
    lines[count * 4 + 7] = -1;

    return lines;
}

int *detect_squares(Image *img, float size_line)
{
	int *horizontals = detect_lines(img, size_line, 0);
	int *verticals = detect_lines(img, size_line, 1);

	if (!horizontals || !verticals || horizontals[0] == -1 || verticals[0] == -1)
	{
		free(horizontals);
		free(verticals);
		return NULL;
	}

	int nb_lines = 0;
	for (int i = 0; horizontals[i] != -1; i+=4)
		nb_lines += 1;

	int *squares = malloc(4 * 81 * sizeof(int));

	int count = 0;

	if (nb_lines == 10) // The sudoku is boxed
	{
		for (int i = 0; horizontals[i + 4] != -1; i+=4)
		{
			for (int j = 0; verticals[j + 4] != -1; j+=4)
			{
				squares[count * 4 + 0] = verticals[j + 2] + 1;
				squares[count * 4 + 1] = horizontals[i + 3] + 1;
				squares[count * 4 + 2] = verticals[j + 4] - 1;
				squares[count * 4 + 3] = horizontals[i + 5] - 1;
				count++;
			}
		}
	}
	else if (nb_lines == 8)
	{
		int last_hor = 0;
		for (int i = 0; horizontals[i] != -1; i+=4)
			last_hor += 4;

		int last_vert = 0;
		for (int j = 0; verticals[j] != -1; j+=4)
			last_vert += 4;

        for (int i = 0; horizontals[i + 4] != -1; i+=4)
        {
			int ti = i - 1;
            for (int j = 0; verticals[j + 4] != -1; j+=4)
            {
				int tj = j - 1;
				
				if (tj >= 0)
					squares[count * 4 + 0] = verticals[tj + 2] + 1;
				else
					squares[count * 4 + 0] = horizontals[0] - 1;

				if (ti >= 0)
					squares[count * 4 + 1] = horizontals[ti + 3] + 1;
				else
					squares[count * 4 + 1] = verticals[1] - 1;

				if (tj <= 9)
					squares[count * 4 + 2] = verticals[tj + 4] - 1;
				else
					squares[count * 4 + 2] = horizontals[last_hor + 2] + 1;

                if (ti <= 9)
					squares[count * 4 + 3] = horizontals[ti + 5] - 1;
				else
					squares[count * 4 + 3] = verticals[last_vert + 3] + 1;
                count++;
            }
        }
	}
	else
	{
		free(horizontals);
		free(verticals);

		free(squares);

		return NULL;
	}

	free(horizontals);
	free(verticals);

	return squares;
}

int *get_bbox(int *matrix, int w, int h, int *x0, int *y0, int *x1, int *y1)
{
	*x0 = w;
	*y0 = h;
	*x1 = -1;
	*y1 = -1;

	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			if (matrix[y * w + x])
			{
				if (x < *x0) *x0 = x;
				if (x > *x1) *x1 = x;
				if (y < *y0) *y0 = y;
				if (y > *y1) *y1 = y;
			}
		}
	}

	// No pixel found
	if (*x1 == -1)
	{
		return NULL;
	}

	int bw = *x1 - *x0 + 1;
	int bh = *y1 - *y0 + 1;

	int *bbox = malloc(sizeof(int) * bw * bh);
	if (!bbox)
		return NULL;

	for (int y = 0; y < bh; y++)
	{
		for (int x = 0; x < bw; x++)
		{
			bbox[y * bw + x] =
				matrix[(*y0 + y) * w + (*x0 + x)];
		}
	}

	return bbox;
}

int **get_squares_sudoku(Image *img)
{
    int *squares_coordinates = detect_squares(img, 0.3);
	if (!squares_coordinates)
	{
		return NULL;
	}

    int **squares = malloc(81 * sizeof(int *));

    for (int i = 0; i < 81; i++)
    {
/*
        printf("group %d : x0 = %d, y0 = %d, x1 = %d, y1 = %d\n",
		        i, squares_coordinates[4 * i + 0], squares_coordinates[4 * i + 1],
		        squares_coordinates[4 * i + 2], squares_coordinates[4 * i + 3]);
*/
		int border_w = 3;

		squares[i] = coordinates_to_matrix(img,
											squares_coordinates[4 * i + 0] + border_w,
											squares_coordinates[4 * i + 1] + border_w,
											squares_coordinates[4 * i + 2] - border_w,
											squares_coordinates[4 * i + 3] - border_w);
		int *save = squares[i];

		int x0 = 0;
		int y0 = 0;
        int x1 = 0;
        int y1 = 0;

		int *bbox = get_bbox(squares[i],
								squares_coordinates[4 * i + 2] - squares_coordinates[4 * i + 0] + 1 - 2 * border_w,
								squares_coordinates[4 * i + 3] - squares_coordinates[4 * i + 1] + 1 - 2 * border_w,
								&x0, &y0, &x1, &y1);

		if (!bbox)
		{
			squares[i] = matrix_to_28x28(squares[i],
                                squares_coordinates[4 * i + 2] - squares_coordinates[4 * i + 0] + 1 - 2 * border_w,
                                squares_coordinates[4 * i + 3] - squares_coordinates[4 * i + 1] + 1 - 2 * border_w);
		}

		else
			squares[i] = matrix_to_28x28(bbox, (x1 - x0 + 1), (y1 - y0 + 1));

        free(save);
		free(bbox);
		//print_matrix(squares[i], 28, 28);
    }

	free(squares_coordinates);
	//img->squares_coordinates = squares_coordinates;

	return squares;
}

int *get_valid_squares(int **squares)
{
	if (!squares) return NULL;

	int *res = malloc(81 * sizeof(int));

	for (int i = 0; i < 81; i++)
    {
        double sum = 0;
		int border_w = 3;
        for (int y = border_w; y < 28 - border_w; y++)
        {
            for (int x = border_w; x < 28 - border_w; x++)
            {
                if (squares[i][y * 28 + x])
                    sum++;
            }
        }

        if ((sum / ((28. - border_w) * (28. - border_w))) > 0.03)
			res[i] = 1;
        else
			res[i] = 0;
    }

	return res;
}
