#pragma once
#include "../global.h"

typedef Uint16 Move;

#define NoFlag 0b0000
#define EnPassantCaptureFlag 0b0001
#define CastleFlag 0b0010
#define PawnTwoUpFlag 0b0011

#define PromoteToQueenFlag 0b0100
#define PromoteToKnightFlag 0b0101
#define PromoteToRookFlag 0b0110
#define PromoteToBishopFlag 0b0111

Uint8 GetTarget(Move move);

Uint8 GetStart(Move move);

Move GetMove(Uint8 startSquare, Uint8 targetSquare);

bool IsEnPassantCapture(Move move);

bool IsCastle(Move move);

bool IsPawnTwoUp(Move move);

bool IsPromotion(Move move);
