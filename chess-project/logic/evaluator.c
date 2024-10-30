#include "evaluator.h"
#include "pieceMaps.h"

int getMaterial(Board* board, int index);
Uint8 flip(Uint8 pos, bool isWhite);

// Summarize material for a side
int getMaterial(Board* board, int index) {
	int material = 0;
	material += board->Pawns[index].count * PAWN_VALUE;
	material += board->Knights[index].count * KNIGHT_VALUE;
	material += board->Bishops[index].count * BISHOP_VALUE;
	material += board->Rooks[index].count * ROOK_VALUE;
	material += board->Queens[index].count * QUEEN_VALUE;
	return material;
}

// Change the perspective of the board by color
Uint8 flip(Uint8 pos, bool isWhite) {
	return isWhite? pos : (7 - pos / 8) * 8 + pos % 8;
}

// Summarize positional offset scores for pieces
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

// Evaluate the score for a board, in perspective of the current side
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