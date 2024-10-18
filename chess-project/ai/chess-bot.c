#include "chess-bot.h"
#include "../logic/evaluator.h"
#include "moveOrdering.h"

#include <time.h>
#include <stdlib.h>

Board* board;

int n;

int searchAttacks(int alpha, int beta);

void mesureBot(Board* boardIn, int depth) {
	board = boardIn;

	search(depth, -1234567, 1234567);

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

int search(int depth, int alpha, int beta) {
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
			return -1000000;
		}
 
		free(moves);

		return 0;
	}

	orderMoves(board, moves, moveCount);

	for (int i = 0; i < moveCount; i++) {
		MakeMove(board, moves[i]);

		int eval = -search(depth - 1, -beta, -alpha);

		RevokeMove(board, moves[i]);
		if (eval >= beta) {
			free(moves);
			return beta;
		}
		alpha = max(alpha, eval);
	}
	free(moves);

	return alpha;
}

void CalcBestMove(Board* boardIn) {
	n = 0;

	int alpha = -1000000;
	int beta = 1000000;

	board = boardIn;

	Move* moves = (Move*)malloc(sizeof(Move) * 100);
	if (moves == NULL) return;
	int moveCount = generateMoves(board, moves, false);

	if (moveCount == 0)
		return;

	int maxIndex = 0;
	orderMoves(board, moves, moveCount);

	for (int i = 0; i < moveCount; i++) {
		MakeMove(board, moves[i]);

		int eval = -search(5, -beta, -alpha);

		RevokeMove(board, moves[i]);
		if (eval >= beta) {
			//printf("Hello!\n");
			alpha = eval;
			maxIndex = i;
			break;
		}

		if (eval > alpha) {
			alpha = eval;
			maxIndex = i;
		}
	}

	MakeMove(board, moves[maxIndex]);

	free(moves);

	printf("Positions evaluated: %10d\n", n);

	printf("Eval: %5d\n", alpha);
}


