#include "evaluator.h"
#include "piece-maps.h"
#include "rules.h"

int getMaterial(Board* board, int index);
Uint8 flip(Uint8 pos, bool isWhite);

int getMaterial(Board* board, int index) {
	int material = 0;
	material += board->Pawns[index].count * PAWN_VALUE;
	material += board->Knights[index].count * KNIGHT_VALUE;
	material += board->Bishops[index].count * BISHOP_VALUE;
	material += board->Rooks[index].count * ROOK_VALUE;
	material += board->Queens[index].count * QUEEN_VALUE;
	return material;
}

Uint8 flip(Uint8 pos, bool isWhite) {
	return isWhite? pos : (7 - pos / 8) * 8 + pos % 8;
}

int getPositionScore(Board* board, bool isWhite) {
	int index = isWhite ? WhiteIndex : BlackIndex;
	int value = 0;
	for (int i = 0; i < board->Pawns[index].count; i++) {
		value += PAWN_MAP[flip(board->Pawns[index].list[i], isWhite)];
	}
	for (int i = 0; i < board->Knights[index].count; i++) {
		value += KNIGHT_MAP[flip(board->Knights[index].list[i], isWhite)];
	}
	for (int i = 0; i < board->Bishops[index].count; i++) {
		value += BISHOP_MAP[flip(board->Bishops[index].list[i], isWhite)];
	}
	for (int i = 0; i < board->Rooks[index].count; i++) {
		value += ROOK_MAP[flip(board->Rooks[index].list[i], isWhite)];
	}
	for (int i = 0; i < board->Queens[index].count; i++) {
		value += QUEEN_MAP[flip(board->Queens[index].list[i], isWhite)];
	}
	

	return value;
}

int evalBoard(Board* board) {

	int whiteValue = getMaterial(board, WhiteIndex);
	int blackValue = getMaterial(board, BlackIndex);

	int totalMat = whiteValue + blackValue;

	if (totalMat > PAWN_VALUE * 20) {
		whiteValue += KING_MAP_EARLY[flip(board->kingSquare[WhiteIndex], isWhite)];
		blackValue += KING_MAP_EARLY[flip(board->kingSquare[BlackIndex], isWhite)];

	}
	else {
		whiteValue += KING_MAP_LATE[flip(board->kingSquare[WhiteIndex], isWhite)];
		blackValue += KING_MAP_LATE[flip(board->kingSquare[BlackIndex], isWhite)];
	}

	whiteValue += getPositionScore(board, true);
	blackValue += getPositionScore(board, false);


	int value = whiteValue - blackValue;

	int perspective = board->isWhitesMove ? 1 : -1;

	return value * perspective;
}


int evalBoard2(Uint8 board[8][8]) {
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
	return valueWhite - valueBlack + posValueEarlyWhite - posValueEarlyBlack;
}