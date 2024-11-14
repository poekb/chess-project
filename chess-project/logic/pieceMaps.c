#include "pieceMaps.h"

// Gets the offset value for a piece at a specified cell 
int GetPositionValue(Board* board, PieceType type, Uint8 square) {
	bool color = board->isWhitesMove;

	switch (type)
	{
	case Pawn:
		return GetMapValue(PAWN_MAP, square / 8, square % 8, color);
	case Knight:
		return GetMapValue(KNIGHT_MAP, square / 8, square % 8, color);
	case Bishop:
		return GetMapValue(BISHOP_MAP, square / 8, square % 8, color);
	case Rook:
		return GetMapValue(ROOK_MAP, square / 8, square % 8, color);
	case Queen:
		return GetMapValue(QUEEN_MAP, square / 8, square % 8, color);
	}

	return 0;
}

int GetMapValue(int map[], int rank, int file, bool color) {

	// Changes the perspective of the board for black
	if (!color) {
		rank = 7 - rank;
	}

	return map[rank * 8 + file];
}