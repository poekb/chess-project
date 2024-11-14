#include "move.h"

#define TargetMask 0b0000000000111111
#define StartMask  0b0000111111000000

#define FlagMask   0b1111000000000000

Uint8 GetTarget(Move move) {
	return move & TargetMask;
}

Uint8 GetStart(Move move) {
	return (move & StartMask) >> 6;
}

Move GetMove(Uint8 startSquare, Uint8 targetSquare) {
	return (Uint16)startSquare << 6 | targetSquare;
}

bool IsEnPassantCapture(Move move) {
	return (move >> 12) == EnPassantCaptureFlag;
}

bool IsCastle(Move move) {
	return (move >> 12) == CastleFlag;
}

bool IsPawnTwoUp(Move move) {
	return (move >> 12) == PawnTwoUpFlag;
}

bool IsPromotion(Move move) {
	return (move >> 12) >= PromoteToQueenFlag;
}