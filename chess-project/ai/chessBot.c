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

int searchAttacks(int alpha, int beta) {
	if (botQuit) {
		return 0;
	}
	int eval = evalBoard(board);
	if (eval >= beta)
		return beta;
	alpha = max(alpha, eval);

	Move* attackMoves = malloc(sizeof(Move) * 100);
	if (attackMoves == NULL) return 0;
	int moveCount = generateMoves(board, attackMoves, true);

	orderMoves(board, attackMoves, moveCount);

	for (int i = 0; i < moveCount; i++) {

		makeMove(board, attackMoves[i]);

		int eval = -searchAttacks(-beta, -alpha);

		unmakeMove(board, attackMoves[i]);
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

	Move* moves = malloc(sizeof(Move) * 218);
	if (moves == NULL) return 0;
	int moveCount = generateMoves(board, moves, false);

	if (moveCount == 0) {
		free(moves);

		if ((((Uint64)1 << board->kingSquare[board->isWhitesMove ? WhiteIndex : BlackIndex]) & board->underAttackMap) != 0)
		{
			int mateScore = MateScore - distFromRoot; // higher eval for closest mate
			return -mateScore;
		}
		return 0;
	}

	orderMoves(board, moves, moveCount);

	Move bestMoveThisPos = 0;

	Uint8 transEvalType = TranspositionUpper;

	for (int i = 0; i < moveCount; i++) {
		makeMove(board, moves[i]);

		int eval = -search(depth - 1, distFromRoot + 1, -beta, -alpha);

		unmakeMove(board, moves[i]);

		if (eval >= beta) {
			insertTransposition(board->zobristHash, beta, distFromRoot, depth, TranspositionLower, moves[i]);
			free(moves);
			return beta;
		}

		if (botQuit) {
			free(moves);
			return 0;
		}

		if (eval > alpha) {
			bestMoveThisPos = moves[i];
			alpha = eval;
			transEvalType = TranspositionExact;
			if (distFromRoot == 0) {
				bestMoveThisIter = moves[i];
			}
		}

	}
	free(moves);
	
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
	while (!botQuit) {
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