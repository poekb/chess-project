#pragma once
#include "../global.h"
#include "piece.h"

#define WhiteKingSide 0b0010
#define WhiteQueenSide 0b0001
#define BlackKingSide 0b1000
#define BlackQueenSide 0b0100

#define ClearWhiteKingSide 0b1101
#define ClearWhiteQueenSide 0b1110
#define ClearBlackKingSide 0b0111
#define ClearBlackQueenSide 0b1011

typedef struct GameState {
	Uint8 enpassantFile;
	Uint8 castleRights;
	PieceType capturedPiece;
	Uint8 halfmoveClock;
} GameState;

bool HasKingsideCastleRight(GameState gameState, bool white);
bool HasQueensideCastleRight(GameState gameState, bool white);