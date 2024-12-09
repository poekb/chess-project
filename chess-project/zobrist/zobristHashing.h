#include "../global.h"


extern Uint64 zobristPieceHashes[64][12]; // 64 squares 6 * 2 piece types

extern Uint64 zobristBlacksTurn;

extern Uint64 zobristCastlingRights[16];

extern Uint64 zobristEnpassant[8];

void InitZobrist();

void LogUint64(Uint64 num);
