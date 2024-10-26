#include "pieceMaps.h"

int getPositionValue(Board* board, PieceType type, Uint8 square) {
	bool color = board->isWhitesMove;

	switch (type)
	{
	case Pawn:
		return getMapValue(PAWN_MAP, square / 8, square % 8, color);
	case Knight:
		return getMapValue(KNIGHT_MAP, square / 8, square % 8, color);
	case Bishop:
		return getMapValue(BISHOP_MAP, square / 8, square % 8, color);
	case Rook:
		return getMapValue(ROOK_MAP, square / 8, square % 8, color);
	case Queen:
		return getMapValue(QUEEN_MAP, square / 8, square % 8, color);
	}

	return 0;
}

int getMapValue(int map[], int rank, int file, bool color) {
	if (!color) {
		rank = 7 - rank;
	}

	return map[rank * 8 + file];
}