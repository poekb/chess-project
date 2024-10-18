#include "../global.h"
#include "../core/piece.h"

#include <SDL_image.h>

void SVG_init(SDL_Renderer *renderer);
void SVG_clear();

void SVG_renderPiece(SDL_Renderer* renderer, Piece piece, int pos_x, int pos_y, double cellSize);