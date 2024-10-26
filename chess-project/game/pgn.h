#include "../global.h"

#include "../core/board.h"
#include "../core/move.h"
#include "../core/piece.h"

char* getPGN(Board* board);
void loadGameFromPGN(Board* board, char* PGN);