#pragma once
#include "../global.h"
#include "piece.h"
#include "pieceList.h"
#include "move.h"
#include "gameState.h"

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

    Uint64 zobristHash;
    Uint64 zobristHistory[10000];
    // State of the game (castling, enpassant, last captured piece)
    GameState currentGameState;
    GameState gameStateHistory[10000]; // there can't be a chess game this long, so it should be sufficient
    Uint16 gameStateHistoryCount;
    bool isWhitesMove;

    Uint64 underAttackMap;

    Uint16 fullmoveClock;
    bool hasGameEnded;
    bool winnerWhite;
    bool winnerBlack;
} Board;

void MakeMove(Board* board, Move move);

void UnmakeMove(Board* board, Move move);

void MakePieceAtSquare(Board* board, Uint8 square, PieceType type, Uint8 colorIndex);

Uint64 GetZobistOfMove(Board* board, Move move);

bool IsCheckPos(Board* board);
