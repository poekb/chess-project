#pragma once
#include "../global.h"
#include "../core/move.h"
#include "../core/board.h"
#include "../moveGeneration/moveGenerator.h"

#define DefaultBoardFEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

typedef struct MoveList {
    Move move;
    struct MoveList* next;
} MoveList;

typedef struct GameData {
    Uint8 selectedPos;
    Uint8 howerPos;

    Move validMoves[100];
    Move move;
    int moveCount;

    bool bot;

    bool isPromotingPiece;
    Move promotingPieceMove;

    char startFEN[100];
    Board* board;
    MoveList* moveHistory;
    MoveList* moveFuture;

    bool nextEnabled;
    bool prevEnabled;
    bool gameLoadEnabled;
} GameData;


void UpdateLoop();

void ToggleBot();

void StashMove(Move move, GameData* gameState);

void LoadFEN(char* fenStr, GameData* gameState);

void UpdateHasGameEnded(GameData* gameState);

void NextMove();

void PrevMove();

void FirstMove();

void LastMove();

void PasteFEN();

void PastePGN();

void CopyFEN();

void CopyPGN();

void SavePGN();

void LoadPGN();

void ResetBoard();

Move GetNextMove(GameData* gameState);

extern bool* nextEnabled;
extern bool* prevEnabled;
extern bool* gameLoadEnabled;
extern Sint32 mouseX;
extern Sint32 mouseY;
