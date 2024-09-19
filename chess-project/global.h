#include <SDL.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef GLOBAL
#define GLOBAL

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

typedef enum Castle {
	WHITE_KING_MOVED = 1,
	WHITE_ROOKL_MOVED = 2,
	WHITE_ROOKR_MOVED = 4,
	
	BLACK_KING_MOVED = 8,
	BLACK_ROOKL_MOVED = 16,
	BLACK_ROOKR_MOVED = 32

} Castle;

typedef struct GamePosition {
	Turn turn;
	Uint8 board[8][8];
	Uint8 enPassant; // Ha nagyobb mint 7, akkor az előző lépésben nem lépett duplát gyalog
	Uint8 castleConditions; // A 'Castle' enum-mal ad értelmet
} GamePosition;

typedef struct Pos {
	Uint8 file;
	Uint8 rank;
}Pos;

SDL_Window* window;

typedef struct PossibleMove {
	Pos pos;
	struct PossibleMove* next;
}PossibleMove;

#endif