#include "../global.h"

#include "../core/board.h"
#include "../core/move.h"
#include "../core/piece.h"
#include "gameHandler.h"


char* GetPGN(Board* board, GameData* gameState);
void LoadGameFromPGN(Board* board, char* PGN, GameData* gameState);