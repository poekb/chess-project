#pragma once
#include "../global.h"

typedef struct PieceList {
    Uint8 list[10];
    Uint8 count;

    Uint8 map[64];
} PieceList;

void InitPieceList(PieceList* pieceList);

void AddPieceListAtSquare(PieceList* piecelist, Uint8 square);

void RemovePieceListAtSquare(PieceList* pieceList, Uint8 square);

void MovePieceList(PieceList* pieceList, Uint8 startSquare, Uint8 targetSquare);
