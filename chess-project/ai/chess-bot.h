#include "../global.h"
#include "../core/move.h"
#include "../logic/game-handler.h"
#include "../logic/evaluator.h"
#include "../core/board.h"
#include "../moveGeneration/moveGenerator.h"

typedef struct EvalCase {
	int value;
	Pos from;
	Pos to;
}EvalCase;

void CalcBestMove(Board* board);