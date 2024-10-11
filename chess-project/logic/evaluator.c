#include "evaluator.h"
#include "piece-maps.h"
#include "rules.h"

int evalBoard(Uint8 board[8][8]) {
	int valueWhite = 0;
	int valueBlack = 0;

	int posValueEarlyWhite = 0;
	int posValueEarlyBlack = 0;

	for (int rank = 0; rank < 8; rank++) {
		for (int file = 0; file < 8; file++) {
			Uint8 piece = board[rank][file];

			if(piece == 0) continue;

			bool color = getColor(piece);


			Uint8 pieceWithoutColor = color ? piece: piece+6;

			int* value = color ? &valueWhite : &valueBlack;
			int* positionValue_early = color ? &posValueEarlyWhite : &posValueEarlyBlack;


			switch (pieceWithoutColor) {
			case PAWN_L:
				*value += PAWN_VALUE;
				*positionValue_early += getMapValue(PAWN_MAP, rank, file, color);
				break;
			case KNIGHT_L:
				*value += KNIGHT_VALUE;
				*positionValue_early += getMapValue(KNIGHT_MAP, rank, file, color);
				break;
			case BISHOP_L:
				*value += BISHOP_VALUE;
				*positionValue_early += getMapValue(BISHOP_MAP, rank, file, color);
				break;
			case ROOK_L:
				*value += ROOK_VALUE;
				*positionValue_early += getMapValue(ROOK_MAP, rank, file, color);
				break;
			case QUEEN_L:
				*value += QUEEN_VALUE;
				*positionValue_early += getMapValue(QUEEN_MAP, rank, file, color);
				break;
			case KING_L:
				*positionValue_early += getMapValue(KING_MAP_EARLY, rank, file, color);
				break;
			}
		}
	}
	printf("%d\n", valueWhite);
	printf("%d\n", valueBlack);
	return valueWhite - valueBlack + posValueEarlyWhite - posValueEarlyBlack;
}