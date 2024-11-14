#include "moveOrdering.h"
#include "../logic/pieceMaps.h"
#include "../core/board.h"
#include "transpositionTable.h"
#include "chessBot.h"
int Partition(int low, int high);
void QuickSort(int low, int high);
int ValueMove(Board* board, Move move);

int moveValues[218];
Move* moves;
int boardEval;

/*
	Order possible moves using the transposition table and a move value prediction
*/
void OrderMoves(Board* board, Move* movesIn, int moveCount) {
	boardEval = -EvalBoard(board);
	moves = movesIn;
	for (int i = 0; i < moveCount; i++) {
		moveValues[i] = -ValueMove(board, moves[i]);
	}

	QuickSort(0, moveCount - 1);
}
/*
	Predicts the value of a move
*/
int ValueMove(Board* board, Move move) {

	Uint64 zobrist = ZobistOfMove(board, move);

	int transposeval = GetRawTransposition(zobrist);
	if (transposeval != TranspositionNotFound) {
		return -(transposeval - boardEval);
	}

	Uint8 targetSquare = GetTarget(move);
	Uint8 startSquare = GetStart(move);


	PieceType targetType = GetPieceType(board->square[targetSquare]);
	PieceType startType = GetPieceType(board->square[startSquare]);

	int moveValue = 0;

	if (targetType != None) {
		moveValue += PieceTypeValues[targetType-1];
	}

	if ((((Uint64)1 << targetSquare) & board->underAttackMap) != 0) {
		moveValue -= PieceTypeValues[startType - 1];
	}


	moveValue += GetPositionValue(board, startType, targetSquare);
	moveValue -= GetPositionValue(board, startType, startType);

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

int Partition(int low, int high) {

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

void QuickSort(int low, int high) {
	if (low < high) {

		int pi = Partition(low, high);

		QuickSort(low, pi - 1);
		QuickSort(pi + 1, high);
	}
}