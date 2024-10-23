#include "../global.h"
#include "piece.h"

#ifndef GAME_STATE
#define GAME_STATE

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
	Uint16 halfmoveClock;
} GameState;

#endif

bool hasKingsideCastleRight(GameState gameState, bool white);
bool hasQueensideCastleRight(GameState gameState, bool white);