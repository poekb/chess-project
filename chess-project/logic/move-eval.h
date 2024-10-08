#include "../global.h"

Move* getPossibleMoves(GamePosition* gamePosition, Pos selectedPos);

bool isCheck(GamePosition* position);

Move* containsMove(Move* moves, Pos move);
void freeMoves(Move* moves);