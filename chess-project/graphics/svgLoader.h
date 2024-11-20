#pragma once
#include "../global.h"
#include "../core/piece.h"

#include <SDL_image.h>

void SVG_Init(SDL_Renderer *renderer);
void SVG_Clear();

void SVG_RenderPiece(SDL_Renderer* renderer, Piece piece, SDL_Rect rect);