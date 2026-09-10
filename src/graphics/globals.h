#ifndef GLOBALS_H
#define GLOBALS_H

#include <SDL2/SDL.h>

extern int running;

extern int mouse_used;
extern int left_pressed;
extern int mouse_hover;

extern const int WINDOW_DEFAULT_W;
extern const int WINDOW_DEFAULT_H;

extern int WIN_W;
extern int WIN_H;
extern int WIN_MIN;

extern int DELTA_W;
extern int DELTA_H;
extern int DELTA;

extern const float WINDOW_MIN_W;
extern const float WINDOW_MIN_H;

extern const float BAR_W;

extern const float DEFAULT_BORDER_W;
extern const float DEFAULT_SPACE_W;

extern const float FONT_SIZE1;
extern const float FONT_SIZE2;
extern const float FONT_SIZE3;
extern const float FONT_SIZE4;
extern const float FONT_SIZE5;
extern const float FONT_SIZE6;
extern const float FONT_SIZE7;
extern const float FONT_SIZE8;
extern const float FONT_SIZE9;
extern const float FONT_SIZE10;

extern SDL_Cursor* arrowCursor;
extern SDL_Cursor* handCursor;
extern SDL_Cursor* textCursor;

extern SDL_Window *Window;
extern SDL_Renderer *Renderer;

extern SDL_Color Red;
extern SDL_Color Green;
extern SDL_Color Blue;
extern SDL_Color White;
extern SDL_Color Black;
extern SDL_Color Gray;
extern SDL_Color DarkGray;
extern SDL_Color LightGray;
extern SDL_Color Yellow;
extern SDL_Color Orange;
extern SDL_Color DarkOrange;
extern SDL_Color Purple;
extern SDL_Color Violet;
extern SDL_Color Magenta;
extern SDL_Color Pink;
extern SDL_Color Cyan;
extern SDL_Color Turquoise;
extern SDL_Color Brown;
extern SDL_Color DarkBrown;
extern SDL_Color Lime;
extern SDL_Color Olive;
extern SDL_Color Navy;
extern SDL_Color SkyBlue;
extern SDL_Color RoyalBlue;
extern SDL_Color Gold;
extern SDL_Color Silver;
extern SDL_Color Beige;
extern SDL_Color Ivory;
extern SDL_Color Coral;
extern SDL_Color Salmon;
extern SDL_Color Crimson;
extern SDL_Color Maroon;
extern SDL_Color Mint;
extern SDL_Color Lavender;

#endif
