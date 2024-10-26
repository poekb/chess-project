#include "../logic/evaluator.h"
#include "chessBot.h"
#include "moveOrdering.h"
#include <stdlib.h>

#include "../game/notations.h"

Board* board;

int n;

int searchAttacks(int alpha, int beta);

void mesureBot(Board* boardIn, int depth) {
	board = boardIn;

	search(depth, 0, -999999999, 999999999);

	printf("Iterations: %d\n", n);
}

int searchAttacks(int alpha, int beta) {
	n++;
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

		RevokeMove(board, attackMoves[i]);
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

	if (depth == distFromRoot) {
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
			return -1000000 + distFromRoot; // higher eval for closest mate
		}
 
		free(moves);

		return 0;
	}

	orderMoves(board, moves, moveCount);

	for (int i = 0; i < moveCount; i++) {
		MakeMove(board, moves[i]);

		int eval = -search(depth, distFromRoot + 1,  -beta, -alpha);

		RevokeMove(board, moves[i]);

		if (eval >= beta) {
			free(moves);
			return beta;
		}

		
		if (eval > alpha) {
			alpha = eval;

			if (distFromRoot == 0) {
				
				bestMove = moves[i];
				bestEval = eval;
			}
		}
		
	}
	free(moves);

	return alpha;
}

Move findBestMove(Board* boardIn) {
	n = 0;
	bestMove = 0;

	search(6, 0, -999999999, 999999999);

	printf("Positions evaluated: %10d\n", n);

	return bestMove;
}


