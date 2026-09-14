#include "ocr/process_img/detection.h"
#include "ocr/process_img/image.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "ocr/useful/matrix.h"
#include "ocr/useful/stack.h"

void set_rectangles(Image *img)
{
    if (img->nb_puzzle_rectangle > 0)
    {
        clean_around_puzzle(img, 20);
    }

	img->rectangles = get_rectangles(img);

	if (img->nb_rectangle < 16)
	{
		for (int i = 0; i < img->nb_rectangle; i++)
		{
			destroy_image_rect(img->rectangles[i]);
		}
		img->nb_rectangle = 0;

		binarize_threshold_mask(img, img->pixels, 220);

		// This time with a higher threshold
		img->rectangles = get_rectangles(img);
	}
}

ImageRect **get_rectangles(Image *img)
{
	if (!img)
		return NULL;

	int rectangles_size = 32;

	ImageRect **rectangles = malloc(sizeof(ImageRect *) * rectangles_size);

	uint8_t *visited = calloc(img->width * img->height, sizeof(uint8_t));

	img->nb_rectangle = 0;

	for (int y = 0; y < img->height; y++)
	{
		for (int x = 0; x < img->width; x++)
		{
			int idx = y * img->width + x;

			// In puzzle rect (because we found it we ignore it)
			if (img->px0 <= x &&  x <= img->px1 && img->py0 <= y && y <= img->py1)
			{
				continue;
			}
			else if (img->pixels[idx] == 1 && !visited[idx])
			{
				if (img->nb_rectangle >= rectangles_size)
				{
					rectangles_size *= 2;

					rectangles = realloc(rectangles, sizeof(ImageRect *) * rectangles_size);
				}

				ImageRect *rect = dfs(img, x, y, visited);

				if (rect)
				{
					// Erase too big and too small rectangles
					if ((img->width * img->height / 10 <= rect->w * rect->h) || (rect->w * rect->h <= 25))
					{
						mask_puzzle_rectangles(img, rect);
						destroy_image_rect(rect);
					}
					else
					{
						rectangles[img->nb_rectangle] = rect;
						img->nb_rectangle++;
						// print_image_rect(rect);
					}
				}
			}
		}
	}

	free(visited);

	return rectangles;
}

ImageRect *dfs(Image *img, int x, int y, uint8_t *visited)
{
	if (!img || !visited)
		return NULL;

	if (x < 0 || y < 0 || x >= img->width || y >= img->height)
		return NULL;

	if (img->pixels[y * img->width + x] != 1)
		return NULL;

	Stack *s = stack_create();
	Coord coord;

	ImageRect *rect = create_image_rect(img, x, y, x, y);

	visited[y * img->width + x] = 1;
	stack_push(s, x, y);

	while (stack_pop(s, &coord) == 0)
	{
		add_point_image_rect(rect, img, coord.x, coord.y);

		for (int dy = -1; dy <= 1; dy++)
		{
			for (int dx = -1; dx <= 1; dx++)
			{
				if (dx == 0 && dy == 0)
					continue;

				int nx = coord.x + dx;
				int ny = coord.y + dy;

				if (nx < 0 || nx >= img->width || ny < 0 || ny >= img->height)
					continue;

				int idx = ny * img->width + nx;

				if (img->pixels[idx] == 1 && !visited[idx])
				{
					visited[idx] = 1;
					stack_push(s, nx, ny);
				}
			}
		}
	}

	stack_free(s);

	// print_image_rect(rect);

	return rect;
}

int far_rectangle(Image *img, int idx, int margin) // Tell if letters are too close or too far
{
	ImageRect *rect = img->rectangles[idx];

	for (int i = 0; i < img->nb_rectangle; i++)
	{
		if (i != idx && ImageRectDistance(rect, img->rectangles[i]) <= margin)
		{
			return 0;
		}
	}

	return 1;
}

// Get the distance of a rect to another (from their middle)
void get_middle_distances_xy(int **distances_x, int **distances_y, Image *img, int margin)
{
	int size = img->nb_rectangle;

	int margin_x = margin;
	int margin_y = margin;

	for (int i = 0; i < size; i++)
	{
		distances_x[i] = calloc(size, sizeof(int));
		distances_y[i] = calloc(size, sizeof(int));

		if (far_rectangle(img, i, 10))
		{
			ImageRect *rect = img->rectangles[i];

			int middle_x = rect->x0 + (rect->x1 - rect->x0) / 2;
			int middle_y = rect->y0 + (rect->y1 - rect->y0) / 2;
			for (int j = 0; j < size; j++)
			{
				if (i != j)
				{
					ImageRect *target = img->rectangles[j];

					int target_middle_x = target->x0 + (target->x1 - target->x0) / 2;
					int target_middle_y = target->y0 + (target->y1 - target->y0) / 2;

					if (middle_x - margin_x <= target_middle_x && target_middle_x <= middle_x + margin_x)
					{
						int d = target_middle_y - middle_y;
						distances_y[i][j] = (d > 0) ? d : -d;
					}

					if (middle_y - margin_y <= target_middle_y && target_middle_y <= middle_y + margin_y)
					{
						int d = target_middle_x - middle_x;
						distances_x[i][j] = (d > 0) ? d : -d;
					}
				}
			}
		}
	}
}

// Get the most common distance x and y between 2 rectangles
void get_all_xy_distances(int **distances_x, int **distances_y, int *all_x, int *all_y, Image *img, int *cx, int *cy)
{
	int size = img->nb_rectangle;

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size && far_rectangle(img, i, 10); j++)
		{
			all_x[distances_x[i][j]] += 1;
			all_y[distances_y[i][j]] += 1;
		}
	}

	int max_w = img->width / 10;
	int max_h = img->height / 10;

	double maxi = 0;
	int start = 10;
	int common_x = start;
	double attenuate = common_x; // Attenuate longer distances
	for (int i = common_x; i < max_w; i++)
	{
		if (maxi < (all_x[i] / attenuate))
		{
			maxi = all_x[i] / attenuate;
			common_x = i;
		}
		if (i % 10 == 0)
		{
			attenuate += 1;
		}
	}

	maxi = 0;
	int common_y = start;
	attenuate = common_y;
	for (int i = common_y; i < max_h; i++)
	{
		if (maxi < (all_y[i] / attenuate))
		{
			maxi = all_y[i] / attenuate;
			common_y = i;
		}
		if (i % 10 == 0)
		{
			attenuate += 1;
		}
	}

	*cx = common_x;
	*cy = common_y;
}

// Eliminate a lot of dirty rect, find a lot of puzzle rect (not always all)
// and also some word letters (we'll need to get rid of them)
ImageRect **step1_puzzle_rectangles(Image *img)
{
	if (!img)
	{
		printf("Error, get_puzzle_rectangles, no image\n");
		return NULL;
	}

	// Get distances
	int margin = 10;

	int size = img->nb_rectangle;
	int **distances_x = malloc(sizeof(int *) * size);
	int **distances_y = malloc(sizeof(int *) * size);

	get_middle_distances_xy(distances_x, distances_y, img, margin);

	// Get most common distances
	int *all_x = calloc(img->width, sizeof(int));
	int *all_y = calloc(img->height, sizeof(int));

	int common_x = 1;
	int common_y = 1;

	get_all_xy_distances(distances_x, distances_y, all_x, all_y, img, &common_x, &common_y);

	int tmp_size = 32;
	int result_count = 0;
	ImageRect **tmp = malloc(sizeof(ImageRect *) * tmp_size);

	for (int i = 0; i < size; i++)
	{
		int stop = 0;
		for (int j = 0; j < size && stop == 0 && far_rectangle(img, i, 10); j++)
		{
			if (i != j)
			{
				if (tmp_size <= result_count)
				{
					tmp_size *= 2;
					tmp = realloc(tmp, sizeof(ImageRect *) * tmp_size);
				}

				// printf("i: %d, j:%d, x: %d, dy:%d\n", i, j, distances_x[i][j], distances_y[i][j]);

				if (common_x - margin <= distances_x[i][j] && distances_x[i][j] <= common_x + margin)
				{
					for (int j2 = 0; j2 < size; j2++)
					{
						if (common_y - margin <= distances_y[i][j2] && distances_y[i][j2] <= common_y + margin)
						{
							tmp[result_count] = img->rectangles[i];
							result_count += 1;
							stop = 1;
							break;
						}
					}
				}
			}
		}
	}

	img->nb_puzzle_rectangle = result_count;
	ImageRect **result = tmp;

	for (int i = 0; i < size; i++)
	{
		free(distances_x[i]);
		free(distances_y[i]);
	}
	free(distances_x);
	free(distances_y);
	free(all_x);
	free(all_y);

	printf("COUNT: %d, common_x: %d, common_y:%d\n", img->nb_puzzle_rectangle, common_x, common_y);

	return result;
}

// Assume that the image is oriented because we'll do it in
// 4 directions anyway
ImageRect **get_puzzle_rectangles(Image *img)
{
	if (!img)
	{
		printf("Error, get_puzzle_rectangles, no image\n");
		return NULL;
	}

	set_rectangles(img);

	// This step tries with a low threshold
	ImageRect **result = step1_puzzle_rectangles(img);

	// Step 1 failed
	if (img->nb_puzzle_rectangle < 9)
	{	
		return NULL;
	}

	// Get the grid size
	int x0 = -1;
	int y0 = -1;
	int x1 = -1;
	int y1 = -1;

	bounding_box_image_rects(result, img->nb_puzzle_rectangle, &x0, &y0, &x1, &y1);

	int nb_x = find_max_puzzle_rectangles_x(result, img->nb_puzzle_rectangle, y0, y1);
	int nb_y = find_max_puzzle_rectangles_y(result, img->nb_puzzle_rectangle, x0, x1);

	printf("The grid is a: x=%d, y=%d\n", nb_x, nb_y);

	// Erase rectangles to put new ones
	for (int i = 0; i < img->nb_rectangle; i++)
	{
		destroy_image_rect(img->rectangles[i]);
	}
	img->nb_rectangle = 0;
	free(result);

	img->nb_puzzle_rectangle = nb_x * nb_y; // Size of the grid
	result = malloc(sizeof(ImageRect *) * img->nb_puzzle_rectangle);

	// Reconstruct the grid
	int puzzle_w = x1 - x0;
	int puzzle_h = y1 - y0;

	double margin = 3.0;
	double gap = 2.0 * margin;

	double tile_w = (puzzle_w - (nb_x - 1) * gap) / nb_x;
	double tile_h = (puzzle_h - (nb_y - 1) * gap) / nb_y;

	for (int y = 0; y < nb_y; y++)
	{
		for (int x = 0; x < nb_x; x++)
		{
			int x0_ = x0 + x * (tile_w + gap);
			int y0_ = y0 + y * (tile_h + gap);
			int x1_ = x0 + x * (tile_w + gap) + tile_w;
			int y1_ = y0 + y * (tile_h + gap) + tile_h;

			result[y * nb_x + x] = create_image_rect(img, x0_, y0_, x1_, y1_);
		}
	}

	img->px0 = x0;
	img->py0 = y0;
	img->px1 = x1;
	img->py1 = y1;

	return result;
}

ImageRectGroup **get_word_rectangles(Image *img)
{
	if (!img)
	{
		printf("Error, get_word_rectangles, no image\n");
		return NULL;
	}

	img->nb_word_rectangle = 0;
	set_rectangles(img);

	int size_grp = 1;
	ImageRectGroup **result = malloc(sizeof(ImageRectGroup *) * size_grp);

	int margin = 20;
	for (int i = 0; i < img->nb_rectangle; i++)
	{
		if (size_grp <= img->nb_word_rectangle)
		{
			size_grp *= 2;
			result = realloc(result, sizeof(ImageRectGroup *) * size_grp);
		}

		if (!far_rectangle(img, i, margin))
		{
			ImageRect *rect = img->rectangles[i];
			ImageRect *copy = create_image_rect(img, rect->x0, rect->y0, rect->x1, rect->y1);

			if (img->nb_word_rectangle == 0)
			{
				img->nb_word_rectangle += 1;
                result[0] = create_image_rect_group(img, rect->x0, rect->y0, rect->x1, rect->y1);
			}

			ImageRectGroup *last_grp = result[img->nb_word_rectangle - 1];

			if (!(last_grp->nb_char == 0 || ImageRectDistance(last_grp->chars[last_grp->nb_char - 1], rect) <= margin))
			{
				img->nb_word_rectangle += 1;
				result[img->nb_word_rectangle - 1] = create_image_rect_group(img, rect->x0, rect->y0, rect->x1, rect->y1);
			}

			add_to_image_rect_group(result[img->nb_word_rectangle - 1], copy);
		}
	}

    for (int i = 0; i < img->nb_rectangle; i++)
    {
        destroy_image_rect(img->rectangles[i]);
    }
    img->nb_rectangle = 0;

	return result;
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
			bbox[y * bw + x] = matrix[(*y0 + y) * w + (*x0 + x)];
		}
	}

	return bbox;
}

void debug_puzzle_rectangles(Image *img)
{
	if (!img || !img->puzzle_rectangles)
	{
		return;
	}

	for (int r = 0; r < img->nb_puzzle_rectangle; r++)
	{
		ImageRect *rect = img->puzzle_rectangles[r];
		for (int y = 0; y < rect->h; y++)
		{
			for (int x = 0; x < rect->w; x++)
			{
				img->pixels[(rect->y0 + y) * img->width + rect->x0 + x] = 2;
			}
		}
	}
}

void mask_puzzle_rectangles(Image *img, ImageRect *rect)
{
	for (int y = 0; y < rect->h; y++)
	{
		for (int x = 0; x < rect->w; x++)
		{
			if (rect->pixels[y][x] == 1)
			{
				img->pixels[(rect->y0 + y) * img->width + rect->x0 + x] = 0;
			}
		}
	}
}

void clean_around_puzzle(Image *img, int margin)
{
    for (int y = 0; y < img->height; y++)
    {
        for (int x = 0; x < img->width; x++)
        {
            if (x >= img->px0 - margin &&
                x <= img->px1 + margin &&
                y >= img->py0 - margin &&
                y <= img->py1 + margin)
            {
                if (x < img->px0 || x > img->px1 ||
                    y < img->py0 || y > img->py1)
                {
                    img->pixels[y * img->width + x] = 0;
                }
            }
        }
    }
}

void debug_word_rectangles(Image *img)
{
    if (!img || !img->puzzle_rectangles)
    {
        return;
    }

    for (int grp = 0; grp < img->nb_word_rectangle; grp++)
    {
		for (int r = 0; r < img->word_rectangles[grp]->nb_char; r++)
		{
			ImageRect *rect = img->word_rectangles[grp]->chars[r];
			for (int y = 0; y < rect->h; y++)
			{
				for (int x = 0; x < rect->w; x++)
				{
					img->pixels[(rect->y0 + y) * img->width + rect->x0 + x] = 3;
				}
			}
		}
    }
}
