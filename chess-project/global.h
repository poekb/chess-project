#pragma once
#include <SDL.h>
#define min(a,b) a > b ? b : a
#define max(a,b) a > b ? a : b
#include <stdlib.h>
#include <stdbool.h>

//#include "debugmalloc.h" //memory leak detections

typedef struct Pos {
	Uint8 file;
	Uint8 rank;
}Pos;

// These are global, because there can only ever be one window
extern SDL_Window* window;
extern SDL_Renderer* renderer;

