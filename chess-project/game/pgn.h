#include "../global.h"

#include "../core/board.h"
#include "../core/move.h"
#include "../core/piece.h"
#include "gameHandler.h"


char* getPGN(Board* board, GameData* gameState);
void loadGameFromPGN(Board* board, char* PGN, GameData* gameState);