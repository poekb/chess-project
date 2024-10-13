#include "../core/board.h"
#include "../global.h"
#include "../core/gameState.h"
#include "../core/move.h"
#include "../core/piece.h"
#include "../core/pieceList.h"
int generateMoves(Board* boardIn, Move* resultIn, bool onlyAttackIn);

Uint64 generateUnderAttackBitmap(Board* board);