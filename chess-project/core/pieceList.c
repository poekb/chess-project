#include "pieceList.h"

void InitPieceList(PieceList* pieceList) {
    pieceList->count = 0;
}

// Add a piece to a piecelist
void AddPieceListAtSquare(PieceList* pieceList, Uint8 square) {
    pieceList->map[square] = pieceList->count;
    pieceList->list[pieceList->count] = square;

    pieceList->count++;
}

// Remove a piece from a piecelist
void RemovePieceListAtSquare(PieceList* pieceList, Uint8 square) {
    Uint8 pieceIndex = pieceList->map[square];
    pieceList->list[pieceIndex] = pieceList->list[pieceList->count - 1];
    pieceList->map[pieceList->list[pieceIndex]] = pieceIndex;

    pieceList->count--;
}

// Move a piece in a piecelist
void MovePieceList(PieceList* pieceList, Uint8 startSquare, Uint8 targetSquare) {
    Uint8 pieceIndex = pieceList->map[startSquare];
    pieceList->list[pieceIndex] = targetSquare;
    pieceList->map[targetSquare] = pieceIndex;
}
