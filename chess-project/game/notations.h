#pragma once
#include "../global.h"
#include "../core/board.h"
#include "../core/move.h"
#include "../core/piece.h"

char* GetMoveNotation(Board* board, Move move);

Move GetMoveFromNotation(Board* board, char* notation);

int GetFENFromBoard(Board* board, char* FEN);

void LoadBoardFromFEN(Board* board, char* FENString);
