#include "moveOrdering.h"
#include "../logic/piece-maps.h"

void quickSort(int low, int high);

int valueMove(Board* board, Move move) {

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

int moveValues[200];
Move* moves;

void orderMoves(Board* board, Move* movesIn, int moveCount) {
	moves = movesIn;
	for (int i = 0; i < moveCount; i++) {
		moveValues[i] = -valueMove(board, moves[i]);
	}

	quickSort(0, moveCount - 1);
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

	// Traverse arr[low..high] and moves all smaller
	// elements to the left side. Elements from low to 
	// i are smaller after every iteration
	for (int j = low; j <= high - 1; j++) {
		if (moveValues[j] < pivot) {
			i++;
			swapMoves(i, j);
		}
	}

	// Move pivot after smaller elements and
	// return its position
	swapMoves(i+1, high);
	return i + 1;
}

void quickSort(int low, int high) {
	if (low < high) {

		// pi is the partition return index of pivot
		int pi = partition(low, high);

		// Recursion calls for smaller elements
		// and greater or equals elements
		quickSort(low, pi - 1);
		quickSort(pi + 1, high);
	}
}



