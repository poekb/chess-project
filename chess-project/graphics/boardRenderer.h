#include "../global.h"
#include "../core/piece.h"

#include <SDL_ttf.h>
#include <SDL_image.h>

#define COLOR_BLACK 0x5A4030FF
#define COLOR_WHITE 0xFFF0D0FF
#define COLOR_HIGH 0x7070F0A0
#define COLOR_BACKGROUND 0x1A1A20FF


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