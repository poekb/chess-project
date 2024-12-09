#pragma once
#include "../core/board.h"
#include "../core/move.h"

int GenerateMoves(Board* boardIn, Move* resultIn, bool onlyAttackIn);

Uint64 GenerateUnderAttackBitmap(Board* board);
