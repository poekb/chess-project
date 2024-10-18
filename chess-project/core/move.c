#include "move.h"

const Uint16 TargetMask = 0b0000000000111111;
const Uint16 StartMask =  0b0000111111000000;

const Uint16 FlagMask =   0b1111000000000000;

const Uint16 NoFlag = 0b0000;
const Uint16 EnPassantCaptureFlag = 0b0001;
const Uint16 CastleFlag = 0b0010;
const Uint16 PawnTwoUpFlag = 0b0011;

// TODO: leave promotions to last
const Uint16 PromoteToQueenFlag = 0b0100;
const Uint16 PromoteToKnightFlag = 0b0101;
const Uint16 PromoteToRookFlag = 0b0110;
const Uint16 PromoteToBishopFlag = 0b0111;

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