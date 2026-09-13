#include "ocr/useful/utils.h"

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

int solve_puzzle(char *puzzle)
{
	if (!puzzle) {
		printf("Error, solve_puzzle, no puzzle");
		return 1;
	}

	return 0;
}
