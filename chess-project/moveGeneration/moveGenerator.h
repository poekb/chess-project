#include "../core/board.h"

#include "../core/move.h"

int generateMoves(Board* boardIn, Move* resultIn, bool onlyAttackIn);

Uint64 generateUnderAttackBitmap(Board* board);