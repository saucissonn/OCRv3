#include "utils.h"

#include <SDL2/SDL.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

double find_angle(int x1, int y1, int x2, int y2)
{
    double dx = x2 - x1;
    double dy = y2 - y1;

    double angle = atan2(dy, dx);

    return angle * 180.0 / M_PI;
}

void elapsed_time(char *name)
{
    static Uint64 last = 0;

    Uint64 now = SDL_GetPerformanceCounter();

    if (last != 0)
    {
        double ms = (double)(now - last) * 1000.0
                  / SDL_GetPerformanceFrequency();
        printf("%s: %.3f ms\n", name, ms);
        fflush(stdout);
    }

    last = now;
}

double rand_uniform()
{
    return 2.0 * rand() / (double)RAND_MAX - 1.0;
}

static int valid_sudoku_box(int *sudoku, int row, int col, int new_val)
{
    for (int i = 0; i < 9; i++)
    {
        if (sudoku[9 * row + i] == new_val)
            return 0;

        if (sudoku[9 * i + col] == new_val)
            return 0;
    }

    int box_row = (row / 3) * 3;
    int box_col = (col / 3) * 3;

    for (int r = box_row; r < box_row + 3; r++)
    {
        for (int c = box_col; c < box_col + 3; c++)
        {
            if (sudoku[9 * r + c] == new_val)
                return 0;
        }
    }

    return 1;
}

int solve_sudoku(int *sudoku)
{
	for (int row = 0; row < 9; row++)
	{
		for (int col = 0; col < 9; col++)
		{
			if (sudoku[9 * row + col] != 0)
				continue;

			for (int i = 1; i <= 9; i++)
			{
				if (valid_sudoku_box(sudoku, row, col, i) == 1)
				{
					sudoku[9 * row + col] = i;
					if (solve_sudoku(sudoku))
						return 1;

					sudoku[9 * row + col] = 0;
				}
			}
			return 0;
		}
	}

	return 1;
}
