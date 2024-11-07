#include "../global.h"
#include "../core/move.h"
#include "../logic/evaluator.h"
#include "../core/board.h"
#include "../moveGeneration/moveGenerator.h"

#define MateScore 100000000

void mesureBot(Board* boardIn, int depth);

int search(int depth, int distFromRoot, int alpha, int beta);
Move findBestMove(Board* board);
Move startBot(Board* board);
bool isMateEval(int eval);
