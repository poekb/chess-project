#include "../global.h"

#include <SDL_ttf.h>
#include <SDL_image.h>

void rendererInit(SDL_Renderer* renderer);
void rederer_cleanUp();
void renderBoard(SDL_Renderer* renderer, double tableSize);
void renderPieces(SDL_Renderer* renderer, Uint8 board[64]);
void renderDynamic(SDL_Renderer* renderer);
void highlightCell(SDL_Renderer* renderer, Pos pos, SDL_Color color);
void displayEval(SDL_Renderer* renderer, Move2* move);
void highlightCells(SDL_Renderer* renderer, Uint8* positions, Uint8 count, SDL_Color color);
void displayBitboard(SDL_Renderer* renderer, Uint64 bitboard, SDL_Color color);

void sdlInit(int szeles, int magas, SDL_Window** pwindow, SDL_Renderer** prenderer);