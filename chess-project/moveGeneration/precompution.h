#include "../global.h"

void preCompute();

extern int Directions[8];

extern Uint8 NumSquaresFromEdge[64][8];

extern Uint8 knightMoves[64][8];
extern Uint8 knightMovesCount[64];

extern Uint8 kingMoves[64][8];
extern Uint8 kingMovesCount[64];

// Bitboards
extern Uint64 queenMoves[64];
extern Uint64 bishopMoves[64];
extern Uint64 rookMoves[64];
extern Uint64 knightAttackBitBoards[64];
extern Uint64 kingAttackBitBoards[64];
extern Uint64 pawnAttackBitBoards[64][2];

extern Uint8 whitePawnAttacks[64][2];
extern Uint8 whitePawnAttacksCount[64];

extern Uint8 blackPawnAttacks[64][2];
extern Uint8 blackPawnAttacksCount[64];