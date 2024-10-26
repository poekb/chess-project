#include "../global.h"
#include "../core/move.h"

void updateLoop();

#ifndef GAME
#define GAME

#define DefaultStart "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

typedef struct MoveList {
    Move move;
    struct MoveList* next;
}MoveList;

#endif
void stashMove(Move move);
void loadFEN(char* fenStr);

void nextMove();
void prevMove();
void pasteFEN();
void pastePGN();
void copyFEN();
void copyPGN();
void savePGN();
void loadPGN();

void resetBoard();

Move getNextMove();

extern char startFEN[];

extern int mouseX;
extern int mouseY;

extern bool nextEnabled;
extern bool prevEnabled;
extern bool gameLoadEnabled;

