#include "move.h"

#define TargetMask 0b0000000000111111
#define StartMask  0b0000111111000000

#define FlagMask   0b1111000000000000

Uint8 getTarget(Move move) {
	return move & TargetMask;
}

Uint8 getStart(Move move) {
	return (move & StartMask) >> 6;
}

Move getMove(Uint8 startSquare, Uint8 targetSquare) {
	return (Uint16)startSquare << 6 | targetSquare;
}

bool isEnPassantCapture(Move move) {
	return (move >> 12) == EnPassantCaptureFlag;
}

bool isCastle(Move move) {
	return (move >> 12) == CastleFlag;
}

bool isPawnTwoUp(Move move) {
	return (move >> 12) == PawnTwoUpFlag;
}

bool isPromotion(Move move) {
	return (move >> 12) >= PromoteToQueenFlag;
}