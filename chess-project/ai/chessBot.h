#pragma once
#include "../global.h"
#include "../core/move.h"
#include "../logic/evaluator.h"
#include "../core/board.h"
#include "../moveGeneration/moveGenerator.h"

#define MateScore 100000000

int Search(int depth, int distFromRoot, int alpha, int beta);
Move FindBestMove(Board* board);
Move StartBot(Board* board);
bool IsMateEval(int eval);
