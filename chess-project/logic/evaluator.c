#include "evaluator.h"
#include "pieceMaps.h"

int GetMaterial(Board* board, int index);
Uint8 Flip(Uint8 pos, bool IsWhite);

// Summarize material for a side
int GetMaterial(Board* board, int index) {
	int material = 0;
	material += board->Pawns[index].count * PAWN_VALUE;
	material += board->Knights[index].count * KNIGHT_VALUE;
	material += board->Bishops[index].count * BISHOP_VALUE;
	material += board->Rooks[index].count * ROOK_VALUE;
	material += board->Queens[index].count * QUEEN_VALUE;
	return material;
}

// Change the perspective of the board by color
Uint8 Flip(Uint8 pos, bool IsWhite) {
	return IsWhite? pos : (7 - pos / 8) * 8 + pos % 8;
}

// Summarize positional offset scores for pieces
int GetPositionScore(Board* board, bool IsWhite) {
	int index = IsWhite ? WhiteIndex : BlackIndex;
	int value = 0;
	for (int i = 0; i < board->Pawns[index].count; i++) {
		value += PAWN_MAP[Flip(board->Pawns[index].list[i], IsWhite)];
	}
	for (int i = 0; i < board->Knights[index].count; i++) {
		value += KNIGHT_MAP[Flip(board->Knights[index].list[i], IsWhite)];
	}
	for (int i = 0; i < board->Bishops[index].count; i++) {
		value += BISHOP_MAP[Flip(board->Bishops[index].list[i], IsWhite)];
	}
	for (int i = 0; i < board->Rooks[index].count; i++) {
		value += ROOK_MAP[Flip(board->Rooks[index].list[i], IsWhite)];
	}
	for (int i = 0; i < board->Queens[index].count; i++) {
		value += QUEEN_MAP[Flip(board->Queens[index].list[i], IsWhite)];
	}
	

	return value;
}

// Evaluate the score for a board, in perspective of the current side
int EvalBoard(Board* board) {

	int whiteValue = GetMaterial(board, WhiteIndex);
	int blackValue = GetMaterial(board, BlackIndex);

	int totalMat = whiteValue + blackValue;

	if (totalMat > PAWN_VALUE * 20) {
		whiteValue += KING_MAP_EARLY[Flip(board->kingSquare[WhiteIndex], IsWhite)];
		blackValue += KING_MAP_EARLY[Flip(board->kingSquare[BlackIndex], IsWhite)];

	}
	else {
		whiteValue += KING_MAP_LATE[Flip(board->kingSquare[WhiteIndex], IsWhite)];
		blackValue += KING_MAP_LATE[Flip(board->kingSquare[BlackIndex], IsWhite)];
	}

	whiteValue += GetPositionScore(board, true);
	blackValue += GetPositionScore(board, false);

	int value = whiteValue - blackValue;

	int perspective = board->isWhitesMove ? 1 : -1;

	return value * perspective;
}