#include "../global.h"

PossibleMove* getPossibleMoves(GamePosition* gamePosition, Pos selectedPos);


bool containsMove(PossibleMove* moves, Pos move);
void clearMoves(PossibleMove* moves);