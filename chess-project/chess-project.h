#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include <SDL.h>
#include <SDL2_gfxPrimitives.h>

typedef enum Piece {
	KING_D = 1,
	QUEEN_D,
	ROOK_D,
	BISHOP_D,
	KNIGHT_D,
	PAWN_D,
	KING_L,
	QUEEN_L,
	ROOK_L,
	BISHOP_L,
	KNIGHT_L,
	PAWN_L
} Piece;

typedef enum Turn {
	WHITE,
	BLACK
} Turn;

typedef struct Position {
	Turn turn;
	Uint8 board[8][8];
} Position;

SDL_Window* window;
