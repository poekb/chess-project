#include "../global.h"
#include "../core/move.h"
#include "../logic/evaluator.h"
#include "../core/board.h"
#include "../moveGeneration/moveGenerator.h"

typedef struct EvalCase {
	int value;
	Pos from;
	Pos to;
}EvalCase;
void mesureBot(Board* boardIn, int depth);

int search(int depth, int alpha, int beta);
Move CalcBestMove(Board* board);