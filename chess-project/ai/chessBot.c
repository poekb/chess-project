#include "../logic/evaluator.h"
#include "chessBot.h"
#include "moveOrdering.h"
#include <stdlib.h>

#include "../game/notations.h"
#include "transpositionTable.h"

Board* board;

int n;
int searchAttacks(int alpha, int beta);

int searchAttacks(int alpha, int beta) {

	int eval = evalBoard(board);
	if (eval >= beta)
		return beta;
	alpha = max(alpha, eval);

	Move* attackMoves = malloc(sizeof(Move) * 100);
	if (attackMoves == NULL) return 0;
	int moveCount = generateMoves(board, attackMoves, true);

	orderMoves(board, attackMoves, moveCount);

	for (int i = 0; i < moveCount; i++) {

		MakeMove(board, attackMoves[i]);

		int eval = -searchAttacks(-beta, -alpha);

		UnmakeMove(board, attackMoves[i]);
		if (eval >= beta) {
			free(attackMoves);
			return beta;
		}
		alpha = max(alpha, eval);
	}
	free(attackMoves);

	return alpha;
}

Move bestMove;
Move bestEval;

int search(int depth, int distFromRoot, int alpha, int beta) {

	if (board->currentGameState.halfmoveClock >= 4) {
		
		for (int i = board->gameStateHistoryCount - board->currentGameState.halfmoveClock; i < board->gameStateHistoryCount; i += 2) {
			if (board->zobristHistory[i] == board->zobristHash)
				return 0; // Evaluate as draw if it is a repetition
		}
	}

	int transposEval = getTransposition(board->zobristHash, distFromRoot, depth, alpha, beta);
	if (transposEval != TranspositionNotFound) {
		n++;
		if (isMateEval(transposEval) && distFromRoot == 0) { // mating sequenc already found
			if (transposEval > bestEval) {
				bestMove = getBestMoveFromTranspos(board->zobristHash);
				bestEval = transposEval;
			}
		}
		return transposEval;
	}

	if (depth == 0) {
		//return evalBoard(board);
		return searchAttacks(alpha, beta);
	}

	Move* moves = malloc(sizeof(Move) * 100);
	if (moves == NULL) return 0;
	int moveCount = generateMoves(board, moves, false);

	if (moveCount == 0) {
		// TODO: Test for checkmate
				
		if ((((Uint64)1 << board->kingSquare[board->isWhitesMove ? WhiteIndex : BlackIndex]) & board->underAttackMap) != 0)
		{
			free(moves);

			int mateScore = MateScore - distFromRoot; // higher eval for closest mate
			return -mateScore; 
		}
 
		free(moves);

		return 0;
	}

	orderMoves(board, moves, moveCount);

	Move bestMoveThisPos = 0;

	Uint8 transEvalType = TranspositionUpper;

	for (int i = 0; i < moveCount; i++) {
		MakeMove(board, moves[i]);

		int eval = -search(depth - 1, distFromRoot + 1,  -beta, -alpha);

		UnmakeMove(board, moves[i]);

		if (eval >= beta) {
			insertTransposition(board->zobristHash, beta, distFromRoot, depth, TranspositionLower, moves[i]);
			free(moves);
			return beta;
		}

		
		if (eval > alpha) {
			bestMoveThisPos = moves[i];
			alpha = eval;
			transEvalType = TranspositionExact;
			if (distFromRoot == 0) {
				
				bestMove = moves[i];
				bestEval = eval;
			}
		}
		
	}

	insertTransposition(board->zobristHash, alpha, distFromRoot, depth, transEvalType, bestMoveThisPos);

	free(moves);

	return alpha;
}

Move findBestMove(Board* boardIn) {

	n = 0;
	bestMove = 0;
	Uint64 oldTick = SDL_GetTicks64();

	int maxTime = 1000;
	int depth = 1;
	int eval = 0;
	while ((SDL_GetTicks64() - oldTick) < maxTime) {
		eval = search(depth, 0, -MateScore, MateScore);
		depth++;
		if (isMateEval(eval) && eval > 0)
			break;

	}
	


	Uint64 tick = SDL_GetTicks64();

	printf("Transpositions found: %10d\n", n);
	printf("Eval: %10d in %d ms\nWith depth of %d\n", eval, (int)(tick - oldTick), depth - 1);
	oldTick = tick;

	return bestMove;
}


bool isMateEval(int eval) {

	if (abs(eval) > MateScore + 1) return false;

	return (abs(eval) > MateScore - 999);
}


