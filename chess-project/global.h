#include <SDL.h>
#include <stdlib.h>
#include <stdbool.h>

//#include "debugmalloc.h" //memory leak detections

#ifndef GLOBAL
#define GLOBAL

typedef struct Pos {
	Uint8 file;
	Uint8 rank;
}Pos;

SDL_Window* window;
SDL_Renderer* renderer;

#endif