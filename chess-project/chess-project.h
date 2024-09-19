#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include <SDL.h>
#include <SDL2_gfxPrimitives.h>

typedef enum {
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

typedef struct Pos {
	Uint8 row;
	Uint8 col;
}Pos;

SDL_Window* window;
