#include "../global.h"

#ifndef MOVE
#define MOVE

typedef Uint16 Move;

#define NoFlag 0b0000
#define EnPassantCaptureFlag 0b0001
#define CastleFlag 0b0010
#define PawnTwoUpFlag 0b0011

#define PromoteToQueenFlag 0b0100
#define PromoteToKnightFlag 0b0101
#define PromoteToRookFlag 0b0110
#define PromoteToBishopFlag 0b0111

#endif

Uint8 getTarget(Move move);
Uint8 getStart(Move move);

Move getMove(Uint8 startSquare, Uint8 targetSquare);

bool isEnPassantCapture(Move move);

bool isCastle(Move move);

bool isPawnTwoUp(Move move);

bool isPromotion(Move move);