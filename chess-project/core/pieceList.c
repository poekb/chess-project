#include "pieceList.h"

void initPieceList(PieceList* pieceList) {
	pieceList->count = 0;
}

void addPieceListAtSquare(PieceList* pieceList, Uint8 square) {
	pieceList->map[square] = pieceList->count;
	pieceList->list[pieceList->count] = square;

	pieceList->count++;
}

void removePieceListAtSquare(PieceList* pieceList, Uint8 square) {
	Uint8 pieceIndex = pieceList->map[square];
	pieceList->list[pieceIndex] = pieceList->list[pieceList->count - 1];
	pieceList->map[pieceList->list[pieceIndex]] = pieceIndex;

	pieceList->count--;
}

void movePieceList(PieceList* pieceList, Uint8 startSquare, Uint8 targetSquare) {
	Uint8 pieceIndex = pieceList->map[startSquare];
	pieceList->list[pieceIndex] = targetSquare;
	pieceList->map[targetSquare] = pieceIndex;
}