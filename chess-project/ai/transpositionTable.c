#include "transpositionTable.h"
#include "../zobrist/zobristHashing.h"
#include "chessBot.h"
#define Size 24

Transposition* transpositionTable;

Uint32 indexMask;

int correctMateEvalIn(int eval, int distFromRoot);
int correctMateEvalOut(int eval, int distFromRoot);

// Initialize the transposition table based on the set size
void initTranspositionTable() {

	transpositionTable = calloc(0b1ul << Size, sizeof(Transposition));
	indexMask = ((Uint32) - 1) >> (32 - Size);
}

// Return the best move from an allready evaluated position
Move getBestMoveFromTranspos(Uint64 zobrist) {
	Uint32 index = (Uint32)zobrist & indexMask;
	return transpositionTable[index].bestMove;
}

// Insert transposition, accounting for depth, and the alpha-beta pruning
void insertTransposition(Uint64 zobrist, int eval,Uint8 distFromRoot, Uint8 depth, Uint8 type, Move bestMove){
	Uint32 index = (Uint32)zobrist & indexMask;
	transpositionTable[index].eval = correctMateEvalIn(eval, distFromRoot);
	transpositionTable[index].bestMove = bestMove;
	transpositionTable[index].zobrist = zobrist;
	transpositionTable[index].depth = depth;
	transpositionTable[index].type = type;
}

// Return a saved value, accounting for depth, and the alpha-beta pruning
int getTransposition(Uint64 zobrist, Uint8 distFromRoot, Uint8 depth, int alpha, int beta) {
	Uint32 index = (Uint32)zobrist & indexMask;
	if (transpositionTable[index].zobrist == zobrist && depth <= transpositionTable[index].depth)
	{
		int eval = correctMateEvalOut(transpositionTable[index].eval, distFromRoot);

		if (transpositionTable[index].type == TranspositionExact)
			return eval;

		if (transpositionTable[index].type == TranspositionUpper && eval <= alpha)
			return eval;

		if (transpositionTable[index].type == TranspositionLower && eval >= beta)
			return eval;

	}
	return TranspositionNotFound;
}

// Return the transposition data for a zobrist hash
int getRawTransposition(Uint64 zobrist) {
	Uint32 index = (Uint32)zobrist & indexMask;
	if (transpositionTable[index].zobrist == zobrist) {
		return transpositionTable[index].eval;
	}
	return TranspositionNotFound;
}

// Correct the evaluation score for mating, because the depth that the transposition will be read at can change
int correctMateEvalIn(int eval, int distFromRoot) {
	if (isMateEval(eval)) {
		int sign = (eval > 0) ? 1 : -1;
		return (eval * sign + distFromRoot) * sign;
	}
	return eval;
}

// Correct the evaluation score for mating, because the depth that the transposition will be read at can change
int correctMateEvalOut(int eval, int distFromRoot) {
	if (isMateEval(eval)) {

		int sign = (eval > 0) ? 1 : -1;

		return (eval * sign - distFromRoot) * sign;
	}

	return eval;
}