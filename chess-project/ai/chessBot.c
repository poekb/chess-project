#include "../logic/evaluator.h"
#include "chessBot.h"
#include "moveOrdering.h"
#include <stdlib.h>

#include "../game/notations.h"
#include "transpositionTable.h"
#include "../core/board.h"

Board* board;

int transpositionCount;
int searchAttacks(int alpha, int beta);

bool botQuit = true;

Move movesBuffer[5000]; // static move buffer for fewer mallocs
int movesBufferOffset = 0;

int searchAttacks(int alpha, int beta) {
	if (botQuit) {
		return 0;
	}
	int eval = evalBoard(board);
	if (eval >= beta)
		return beta;
	alpha = max(alpha, eval);

	int moveCount = generateMoves(board, &movesBuffer[movesBufferOffset], true);
	orderMoves(board, &movesBuffer[movesBufferOffset], moveCount);

	for (int i = 0; i < moveCount; i++) {

		makeMove(board, movesBuffer[movesBufferOffset + i]);

		movesBufferOffset += moveCount;
		int eval = -searchAttacks(-beta, -alpha);
		movesBufferOffset -= moveCount;

		unmakeMove(board, movesBuffer[movesBufferOffset + i]);
		if (eval >= beta) {
			return beta;
		}
		alpha = max(alpha, eval);
	}
	return alpha;
}

Move bestMove;

Move bestMoveThisIter;
int bestEval;



int search(int depth, int distFromRoot, int alpha, int beta) {
	if (botQuit) {
		return 0;
	}

	if (board->currentGameState.halfmoveClock >= (distFromRoot == 0) ? 6 : 4 ) {
		int count = 0;
		for (int i = board->gameStateHistoryCount - board->currentGameState.halfmoveClock; i < board->gameStateHistoryCount; i += 2) {
			if (board->zobristHistory[i] == board->zobristHash)
			{
				count++;
				if(count > (distFromRoot == 0) ? 1 : 0)
					return 0; // Evaluate as draw if it is a repetition
			}
		}
	}

	int transposEval = getTransposition(board->zobristHash, distFromRoot, depth, alpha, beta);
	if (transposEval != TranspositionNotFound) {
		transpositionCount++;

		Move transposMove = getBestMoveFromTranspos(board->zobristHash);
		bool isRepetition = false;

		if (board->currentGameState.halfmoveClock >= 3) {
			Uint64 zobrist = zobistOfMove(board, transposMove);
			for (int i = board->gameStateHistoryCount - board->currentGameState.halfmoveClock; i < board->gameStateHistoryCount; i += 2) {
				if (board->zobristHistory[i] == zobrist)
					isRepetition = true; // Evaluate as draw if it is a repetition
			}
		}
		if (!isRepetition) {
			if (distFromRoot == 0) {
				bestMoveThisIter = transposMove;
			}
			return transposEval;
		}
	}

	if (depth == 0) {
		return searchAttacks(alpha, beta);
	}

	int moveCount = generateMoves(board, &movesBuffer[movesBufferOffset], false);

	if (moveCount == 0) {

		if ((((Uint64)1 << board->kingSquare[board->isWhitesMove ? WhiteIndex : BlackIndex]) & board->underAttackMap) != 0)
		{
			int mateScore = MateScore - distFromRoot; // higher eval for closest mate
			return -mateScore;
		}
		return 0;
	}

	orderMoves(board, &movesBuffer[movesBufferOffset], moveCount);


	Move bestMoveThisPos = 0;

	Uint8 transEvalType = TranspositionUpper;

	for (int i = 0; i < moveCount; i++) {
		makeMove(board, movesBuffer[movesBufferOffset + i]);

		movesBufferOffset += moveCount;
		int eval = -search(depth - 1, distFromRoot + 1, -beta, -alpha);
		movesBufferOffset -= moveCount;

		unmakeMove(board, movesBuffer[movesBufferOffset + i]);

		if (eval >= beta) {
			insertTransposition(board->zobristHash, beta, distFromRoot, depth, TranspositionLower, movesBuffer[movesBufferOffset + i]);
			return beta;
		}

		if (botQuit) {
			return 0;
		}

		if (eval > alpha) {
			bestMoveThisPos = movesBuffer[movesBufferOffset + i];
			alpha = eval;
			transEvalType = TranspositionExact;
			if (distFromRoot == 0) {
				bestMoveThisIter = movesBuffer[movesBufferOffset + i];
			}
		}

	}

	insertTransposition(board->zobristHash, alpha, distFromRoot, depth, transEvalType, bestMoveThisPos);

	return alpha;
}

Move findBestMove(Board* boardIn) {
	board = boardIn;
	transpositionCount = 0;
	bestMove = 0;
	bestMoveThisIter = 0;

	int depth = 1;
	int eval = 0;
	while (!botQuit && depth < 20) {
		bestMoveThisIter = 0;
		eval = search(depth, 0, -MateScore, MateScore);

		if (botQuit)
			break;
		depth++;

		bestMove = bestMoveThisIter;
		bestEval = eval;
		if (isMateEval(eval) && eval > 0)
			break;
	}
	printf("depth: %d\n", depth);
	printf("Eval: %10d\n", bestEval);

	return bestMove;
}

Move startBot(Board* board) {
	botQuit = false;

	SDL_Thread* botThread = SDL_CreateThread(findBestMove, "bot", board);

	SDL_Delay(2000);
	botQuit = true;
	SDL_WaitThread(botThread, 0);

	char* note = getMoveNotation(board, bestMove);
	free(note);

	return bestMove;
}

bool isMateEval(int eval) {

	if (abs(eval) > MateScore + 1) return false;

	return (abs(eval) > MateScore - 999);
}