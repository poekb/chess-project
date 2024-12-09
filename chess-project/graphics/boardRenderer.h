#pragma once
#include "../global.h"
#include "../core/piece.h"
#include "../core/move.h"
#include <SDL_ttf.h>
#include <SDL_image.h>

#define COLOR_BLACK 0x5A4030FF
#define COLOR_WHITE 0xFFF0D0FF
#define COLOR_HIGH 0x7070F0A0
#define COLOR_BACKGROUND 0x1A1A20FF

void InitRenderer(SDL_Renderer* renderer);

void CleanupRenderer();

void RenderBoard(SDL_Renderer* renderer, int boardSizeNew, int boardOffsetX, int boardOffsetY);

void RenderPieces(SDL_Renderer* renderer, Piece board[64]);

void RenderStatic(SDL_Renderer* renderer);

void HighlightCell(SDL_Renderer* renderer, Uint8 pos, SDL_Color color);

void HighlightCells(SDL_Renderer* renderer, Uint8* positions, Uint8 count, SDL_Color color);

void DisplayBitboard(SDL_Renderer* renderer, Uint64 bitboard, SDL_Color color);

void DisplayPromotionSelect(SDL_Renderer* renderer, Move move, bool IsWhite);

PieceType GetPromotionPieceTypeByCell(Move move, Uint8 cell, bool IsWhite);

Uint8 GetCell(int x, int y);

void SetDrawColor(SDL_Renderer* renderer, SDL_Color color);

void InitSDL(int szeles, int magas, SDL_Window** pwindow, SDL_Renderer** prenderer);
