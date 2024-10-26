#include "zobristHashing.h"

Uint64 zobristPieceHashes[64][12]; // 64 squares 6 * 2 piece types

Uint64 zobistTurn;

Uint64 zobristCastlingRights[16];

Uint64 zobristEnpassant[8];

