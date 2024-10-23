#include "../global.h"
#include "../core/move.h"

void updateLoop();

#ifndef GAME
#define GAME

typedef struct MoveList {
    Move move;
    struct MoveList* next;
}MoveList;

#endif

void nextMove();
void prevMove();
void pasteFEN();
void copyFEN();
void copyPGN();
void resetBoard();

Move getNextMove();

extern char startFEN[];

extern int mouseX;
extern int mouseY;

extern bool nextEnabled;
extern bool prevEnabled;
extern bool gameLoadEnabled;

