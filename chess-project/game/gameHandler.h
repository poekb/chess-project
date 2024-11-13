#include "../global.h"
#include "../core/move.h"
#include "../core/board.h"
#include "../moveGeneration/moveGenerator.h"

#ifndef GAME
#define GAME

#define DefaultBoardFEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

typedef struct MoveList {
    Move move;
    struct MoveList* next;
}MoveList;

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


void updateLoop();

void toggleBot();

void stashMove(Move move, GameData* gameState);
void loadFEN(char* fenStr, GameData* gameState);

void updateHasGameEnded(GameData* gameState);

void nextMove();
void prevMove();
void firstMove();
void lastMove();

void pasteFEN();
void pastePGN();
void copyFEN();
void copyPGN();
void savePGN();
void loadPGN();

void resetBoard();

Move getNextMove(GameData* gameState);

extern bool* nextEnabled;
extern bool* prevEnabled;
extern bool* gameLoadEnabled;
extern Sint32 mouseX;
extern Sint32 mouseY;

#endif
