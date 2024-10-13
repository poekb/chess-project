#include "../global.h"

Move2* getPossibleMoves(GamePosition* gamePosition, Pos selectedPos);

bool isCheck(GamePosition* position);
bool isAttacked(GamePosition* position, Pos coords, bool turn);

Move2* containsMove(Move2* moves, Pos move);
void freeMoves(Move2* moves);

bool getColor(Uint8 piece);