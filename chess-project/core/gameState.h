#pragma once
#include "../global.h"
#include "piece.h"

#define CastleWhiteKingSide 0b0010
#define CastleWhiteQueenSide 0b0001
#define CastleBlackKingSide 0b1000
#define CastleBlackQueenSide 0b0100

#define ClearCastleWhiteKingSide 0b1101
#define ClearCastleWhiteQueenSide 0b1110
#define ClearCastleBlackKingSide 0b0111
#define ClearCastleBlackQueenSide 0b1011

typedef struct GameState {
    Uint8 enpassantFile;
    Uint8 castleRights;
    PieceType capturedPiece;
    Uint8 halfmoveClock;
} GameState;

bool HasKingsideCastleRight(GameState gameState, bool white);

bool HasQueensideCastleRight(GameState gameState, bool white);
