#include "../global.h"

#include "../core/board.h"
#include "../core/move.h"
#include "../core/piece.h"

char* getMoveNotation(Board* board, Move move);
Move getMoveFromNotation(Board* board, char* notation);

void getFENFromBoard(Board* board, char* FEN);
void LoadBoardFromFEN(Board* board, char* FENString);
