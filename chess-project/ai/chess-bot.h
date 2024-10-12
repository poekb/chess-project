#include "../global.h"

typedef struct EvalCase {
	int value;
	Pos from;
	Pos to;
}EvalCase;

EvalCase miniMax(int depth, GamePosition position, bool inv);
void CalcBestMove(Game* game);