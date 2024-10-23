#include "../global.h"
#include "piece.h"
#include "pieceList.h"
#include "move.h"
#include "gameState.h"



#ifndef BOARD
#define BOARD

extern const int WhiteIndex;
extern const int BlackIndex;

typedef struct Board {
	Piece square[64];

	// Square index of white and black king
	Uint8 kingSquare[2];


	// Piece lists
	PieceList Rooks[2];
	PieceList Bishops[2];
	PieceList Queens[2];
	PieceList Knights[2];
	PieceList Pawns[2];

	// State of the game (castling, enpassant, last captured piece)
	GameState currentGameState;
	GameState gameStateHistory[10000]; // there can't be a chess game this long, so it should be sufficient
	Uint16 gameStateHistoryCount;
	bool isWhitesMove;

	Uint64 underAttackMap;

	Uint16 fullmoveClock;
} Board;

void MakeMove(Board* board, Move move);
void RevokeMove(Board* board, Move move);

bool isCheckPos(Board* board);

#endif