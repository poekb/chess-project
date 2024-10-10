#include "../global.h"

Move* getPossibleMoves(GamePosition* gamePosition, Pos selectedPos);

bool isCheck(GamePosition* position);
bool isAttacked(GamePosition* position, Pos coords, bool turn);

Move* containsMove(Move* moves, Pos move);
void freeMoves(Move* moves);