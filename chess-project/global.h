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

typedef enum Color {
	WHITE = true,
	BLACK = false
} Color;

typedef enum Castle {
	WHITE_KINGSIDE = 1,
	WHITE_QUEENSIDE = 2,
	BLACK_KINGSIDE = 4,
	BLACK_QUEENSIDE = 8,

} Castle;

typedef struct GamePosition {
	Uint8 board[8][8];
	bool turn;
	Uint8 castleConditions; // A 'Castle' enum-mal kap értelmet
	Uint8 enPassant; // Ha nagyobb mint 7, akkor az előző lépésben nem lépett duplát gyalog
	int halfmoveClock;
	int fullmove;
} GamePosition;

typedef struct Pos {
	Uint8 file;
	Uint8 rank;
}Pos;

SDL_Window* window;

typedef struct Move {
	Pos pos;
	Uint8 enPassant;
	bool takesEnPassant;
	struct Move* next;
}Move;

typedef struct Game {
	GamePosition position;

	struct Game* prev;
	struct Game* next;
} Game;

#endif