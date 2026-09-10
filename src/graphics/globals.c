#include "globals.h"

#include <SDL2/SDL.h>

int running = 1;

int mouse_used = 0;
int left_pressed = 0;
int mouse_hover = 0;

const int WINDOW_DEFAULT_W = 1260;
const int WINDOW_DEFAULT_H = 720;

int WIN_W = 1260;
int WIN_H = 720;
int WIN_MIN = 720;

int DELTA_W = 1260;
int DELTA_H = 720;
int DELTA = 720;

const float WINDOW_MIN_W = 400.0;
const float WINDOW_MIN_H = 400.0;

const float BAR_W = 0.4;

const float DEFAULT_BORDER_W = 0.004;
const float DEFAULT_SPACE_W = 0.01;

const float FONT_SIZE1  = 0.01;
const float FONT_SIZE2  = 0.02;
const float FONT_SIZE3  = 0.03;
const float FONT_SIZE4  = 0.04;
const float FONT_SIZE5  = 0.05;
const float FONT_SIZE6  = 0.06;
const float FONT_SIZE7  = 0.07;
const float FONT_SIZE8  = 0.08;
const float FONT_SIZE9  = 0.09;
const float FONT_SIZE10 = 0.10;

SDL_Cursor* arrowCursor = NULL;
SDL_Cursor* handCursor = NULL;
SDL_Cursor* textCursor = NULL;

SDL_Window *Window = NULL;
SDL_Renderer *Renderer = NULL;

SDL_Color Red         = {255,   0,   0, 255};
SDL_Color Green       = {  0, 255,   0, 255};
SDL_Color Blue        = {  0,   0, 255, 255};
SDL_Color White       = {255, 255, 255, 255};
SDL_Color Black       = {  0,   0,   0, 255};
SDL_Color Gray        = {100, 100, 100, 255};
SDL_Color DarkGray    = { 50,  50,  50, 255};
SDL_Color LightGray   = {200, 200, 200, 255};
SDL_Color Yellow      = {255, 255,   0, 255};
SDL_Color Orange      = {255, 165,   0, 255};
SDL_Color DarkOrange  = {255, 140,   0, 255};
SDL_Color Purple      = {128,   0, 128, 255};
SDL_Color Violet      = {238, 130, 238, 255};
SDL_Color Magenta     = {255,   0, 255, 255};
SDL_Color Pink        = {255, 192, 203, 255};
SDL_Color Cyan        = {  0, 255, 255, 255};
SDL_Color Turquoise   = { 64, 224, 208, 255};
SDL_Color Brown       = {139,  69,  19, 255};
SDL_Color DarkBrown   = {101,  67,  33, 255};
SDL_Color Lime        = { 50, 205,  50, 255};
SDL_Color Olive       = {128, 128,   0, 255};
SDL_Color Navy        = {  0,   0, 128, 255};
SDL_Color SkyBlue     = {135, 206, 235, 255};
SDL_Color RoyalBlue   = { 65, 105, 225, 255};
SDL_Color Gold        = {255, 215,   0, 255};
SDL_Color Silver      = {192, 192, 192, 255};
SDL_Color Beige       = {245, 245, 220, 255};
SDL_Color Ivory       = {255, 255, 240, 255};
SDL_Color Coral       = {255, 127,  80, 255};
SDL_Color Salmon      = {250, 128, 114, 255};
SDL_Color Crimson     = {220,  20,  60, 255};
SDL_Color Maroon      = {128,   0,   0, 255};
SDL_Color Mint        = {152, 255, 152, 255};
SDL_Color Lavender    = {230, 230, 250, 255};
