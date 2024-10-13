#include "../global.h"
#include "piece.h"

#ifndef GAME_STATE
#define GAME_STATE

extern const Uint8 ClearWhiteKingSide;
extern const Uint8 ClearWhiteQueenSide;
extern const Uint8 ClearBlackKingSide;
extern const Uint8 ClearBlackQueenSide;

typedef struct GameState {
	Uint8 enpassantFile;
	Uint8 castleRights;
	PieceType capturedPiece;
} GameState;

#endif

bool hasKingsideCastleRight(GameState gameState, bool white);
bool hasQueensideCastleRight(GameState gameState, bool white);