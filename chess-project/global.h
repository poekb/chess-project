#include <SDL.h>
#pragma once
#include <stdlib.h>
#include <stdbool.h>

//#include "debugmalloc.h" //memory leak detections

typedef struct Pos {
	Uint8 file;
	Uint8 rank;
}Pos;

// These are global, because there can only ever be one window
SDL_Window* window;
SDL_Renderer* renderer;

