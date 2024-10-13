#include "chess-bot.h"
#include "../logic/evaluator.h"

#include <time.h>
#include <stdlib.h>

Board* board;

int searchAttacks() {
	
	Move* attackMoves = malloc(sizeof(Move) * 100);
	if (attackMoves == NULL) return 0;
	int moveCount = generateMoves(board, attackMoves, true);

	if (moveCount == 0) {
		// TODO: Test for checkmate
		Uint64 attackMap = generateUnderAttackBitmap(board);
		if ((((Uint64)1 << board->kingSquare[board->isWhitesMove ? BlackIndex : WhiteIndex]) & attackMap) != 0)
		{
			free(attackMoves);
			return -1234567;
		}

		free(attackMoves);
		return evalBoard(board);
	}

	//printf("%d\n", moveCount);

	//if (depth < 0) return evalBoard(board);

	int maxValue = -1234567;

	for (int i = 0; i <moveCount; i++) {

		MakeMove(board, attackMoves[i]);

		maxValue = max(maxValue, -evalBoard(board));

		RevokeMove(board, attackMoves[i]);
	}
	free(attackMoves);

	return maxValue;
}

int search(int depth, int alpha, int beta) {

	if (depth == 0) {
		return searchAttacks();
	}
	Move* moves = malloc(sizeof(Move) * 100);
	if (moves == NULL) return 0;
	int moveCount = generateMoves(board, moves, false);

	if (moveCount == 0) {
		// TODO: Test for checkmate
		Uint64 attackMap = generateUnderAttackBitmap(board);
		if ((((Uint64)1 << board->kingSquare[board->isWhitesMove ? BlackIndex : WhiteIndex]) & attackMap) != 0)
		{
			free(moves);
			return -1234567;
		}

		free(moves);

		return 0;
	}

	int maxValue = -1234567;

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

	board = boardIn;

	Move* moves = malloc(sizeof(Move) * 100);
	if (moves == NULL) return 0;
	int moveCount = generateMoves(board, moves, false);

	if (moveCount == 0)
		return;

	int maxValue = INT_MIN;
	int maxIndex = 0;

	for (int i = 0; i < moveCount; i++) {
		MakeMove(board, moves[i]);

		int eval = -search(3, -1234567, 1234567);
		if (eval > maxValue) {
			maxValue = eval;
			maxIndex = i;
		}

		RevokeMove(board, moves[i]);
	}

	MakeMove(board, moves[maxIndex]);


	printf("Eval: %5d\n", maxValue);
}


