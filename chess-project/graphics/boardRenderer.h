#include "../global.h"
#include "../core/piece.h"

#include <SDL_ttf.h>
#include <SDL_image.h>

#define COLOR_BLACK (SDL_Color){ 0x5A, 0x40, 0x30, 0xFF }
#define COLOR_WHITE (SDL_Color){ 0xFF, 0xF0, 0xD0, 0xFF }
#define COLOR_HIGH (SDL_Color){ 0x70, 0x70, 0xF0, 0xA0 }
#define COLOR_BACKGROUND (SDL_Color){ 0x1a, 0x1a, 0x20, 0xFF }


void rendererInit(SDL_Renderer* renderer);
void redererCleanup();
void renderBoard(SDL_Renderer* renderer, int boardSizeNew, int boardOffsetX, int boardOffsetY);
void renderPieces(SDL_Renderer* renderer, Piece board[64]);
void renderStatic(SDL_Renderer* renderer);
void highlightCell(SDL_Renderer* renderer, Uint8 pos, SDL_Color color);

void highlightCells(SDL_Renderer* renderer, Uint8* positions, Uint8 count, SDL_Color color);
void displayBitboard(SDL_Renderer* renderer, Uint64 bitboard, SDL_Color color);

Uint8 getCell(int x, int y);

void setDrawColor(SDL_Renderer* renderer, SDL_Color color);

void sdlInit(int szeles, int magas, SDL_Window** pwindow, SDL_Renderer** prenderer);