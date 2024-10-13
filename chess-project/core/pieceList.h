#include "../global.h"
#ifndef PIECE_LIST
#define PIECE_LIST

typedef struct PieceList {
	Uint8 list[10];
	Uint8 count;

	Uint8 map[64];
} PieceList;

void initPieceList(PieceList* pieceList);

void addPieceListAtSquare(PieceList* piecelist, Uint8 square);
void removePieceListAtSquare(PieceList* pieceList, Uint8 square);
void movePieceList(PieceList* pieceList, Uint8 startSquare, Uint8 targetSquare);

#endif