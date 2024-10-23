#include "../global.h"

#ifndef MOVE
#define MOVE

typedef Uint16 Move;

extern const Uint16 NoFlag;
extern const Uint16 EnPassantCaptureFlag;
extern const Uint16 CastleFlag;
extern const Uint16 PawnTwoUpFlag;

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