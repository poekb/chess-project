#include "../global.h"

#include "../core/board.h"
#include "../core/move.h"
#include "../core/piece.h"

int getMoveNotation(Board* board, Move move, char* string);
void getFENFromBoard(Board* board, char* FEN);