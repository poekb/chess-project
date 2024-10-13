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
	GameState gameStateHistory[10000]; // a static size stack to store the history of the game states !!!!!!!! the game breaks over 10000 moves!!!!!!!
	int gameStateHistoryCount;
	bool isWhitesMove;
} Board;

void MakeMove(Board* board, Move move);
void RevokeMove(Board* board, Move move);

void LoadBoardFromFEN(Board* board, char* FENString);

#endif