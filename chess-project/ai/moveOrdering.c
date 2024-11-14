#include "moveOrdering.h"
#include "../logic/pieceMaps.h"
#include "../core/board.h"
#include "transpositionTable.h"
#include "chessBot.h"
int partition(int low, int high);
void quickSort(int low, int high);
int valueMove(Board* board, Move move);

int moveValues[218];
Move* moves;
int boardEval;

/*
	Order possible moves using the transposition table and a move value prediction
*/
void orderMoves(Board* board, Move* movesIn, int moveCount) {
	boardEval = -evalBoard(board);
	moves = movesIn;
	for (int i = 0; i < moveCount; i++) {
		moveValues[i] = -valueMove(board, moves[i]);
	}

	quickSort(0, moveCount - 1);
}
/*
	Predicts the value of a move
*/
int valueMove(Board* board, Move move) {

	Uint64 zobrist = zobistOfMove(board, move);

	int transposeval = getRawTransposition(zobrist);
	if (transposeval != TranspositionNotFound) {
		return -(transposeval - boardEval);
	}

	Uint8 targetSquare = getTarget(move);
	Uint8 startSquare = getStart(move);


	PieceType targetType = getPieceType(board->square[targetSquare]);
	PieceType startType = getPieceType(board->square[startSquare]);

	int moveValue = 0;

	if (targetType != None) {
		moveValue += PieceTypeValues[targetType-1];
	}

	if ((((Uint64)1 << targetSquare) & board->underAttackMap) != 0) {
		moveValue -= PieceTypeValues[startType - 1];
	}


	moveValue += getPositionValue(board, startType, targetSquare);
	moveValue -= getPositionValue(board, startType, startType);

	return moveValue;
}

void swapMoves(int a, int b) {
	Move tempMove = moves[a];
	moves[a] = moves[b];
	moves[b] = tempMove;

	int tempVal = moveValues[a];
	moveValues[a] = moveValues[b];
	moveValues[b] = tempVal;
}

int partition(int low, int high) {

	int pivot = moveValues[high];
	
	int i = low - 1;

	for (int j = low; j <= high - 1; j++) {
		if (moveValues[j] < pivot) {
			i++;
			swapMoves(i, j);
		}
	}

	swapMoves(i+1, high);
	return i + 1;
}

void quickSort(int low, int high) {
	if (low < high) {

		int pi = partition(low, high);

		quickSort(low, pi - 1);
		quickSort(pi + 1, high);
	}
}