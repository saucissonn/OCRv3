#include "graphics/frame.h"
#include "graphics/frames/frames.h"
#include "graphics/events.h"
#include "graphics/update.h"
#include "graphics/utils.h"
#include "graphics/globals.h"
#include "ocr/useful/globals_ocr.h"
#include "ocr/neural_network/ocr.h"
#include "ocr/neural_network/save.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <time.h>

int main(void)
{
	srand(time(NULL));

	init_global_variables();

	printf("nb threads = %d\n", NB_THREADS_MAX);

	Frame *frame = create_frame(NULL);
	init_frame_ocr(frame);

    SDL_Event event;

    while (running)
    {		
		handle_events(event, frame);

        SDL_SetRenderDrawColor(Renderer, 30, 30, 30, 255);

        SDL_RenderClear(Renderer);
	
		display_frame(frame);

		update_all(frame);
		update_mouse();
	
		SDL_RenderPresent(Renderer);
	}

	SDL_StopTextInput();

	SDL_FreeCursor(handCursor);
	SDL_FreeCursor(arrowCursor);
	SDL_FreeCursor(textCursor);

	destroy_font();
    destroy_frame(frame);

    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

	printf("GOOD ENDING\n");

    return 0;
}
