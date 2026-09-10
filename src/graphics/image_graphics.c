#include "image_graphics.h"
#include "globals.h"

#include <SDL2/SDL_image.h>
#include <stdio.h>

int load_image(Frame *frame, const char *path)
{
    SDL_Texture *new_texture =
        IMG_LoadTexture(Renderer, path);

    if (!new_texture)
    {
        fprintf(stderr,
                "Loading error: %s\n",
                IMG_GetError());
        return 0;
    }

    if (frame->texture)
        SDL_DestroyTexture(frame->texture);

    frame->texture = new_texture;

    SDL_QueryTexture(
        frame->texture,
        NULL,
        NULL,
        &frame->image_w,
        &frame->image_h);

    return 1;
}

void render_image(Frame *frame)
{
    if (!frame->texture)
        return;

    float scale_x =
	scale_x = (float)(DELTA_W - BAR_W * DELTA_W) / frame->image_w;
    
	float scale_y;
    scale_y = (float)(DELTA_H) / frame->image_h;

    float scale = scale_x < scale_y ? scale_x : scale_y;

	if (scale < 0) {
		scale = 0;
	}

    SDL_Rect dst =
    {
        BAR_W * DELTA_W,
        0,
        (int)(frame->image_w * scale),
        (int)(frame->image_h * scale)
    };

    SDL_RenderCopy(
        Renderer,
        frame->texture,
        NULL,
        &dst);
}
