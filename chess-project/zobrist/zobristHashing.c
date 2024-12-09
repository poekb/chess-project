#include "zobristHashing.h"
#include "randomGenerator.h"

Uint64 zobristPieceHashes[64][12]; // 64 squares 6 * 2 piece types

Uint64 zobristBlacksTurn;

Uint64 zobristCastlingRights[16];

Uint64 zobristEnpassant[8];

void LogUint64(Uint64 num) {
    for (int i = 0; i < 64; i++) {
        putchar(((num >> i) & 1ul) == 0 ? '0' : '1');
    }
    putchar('\n');
}

// Fill up the zobrist hash values with random Uint64s
void InitZobrist() {
    Uint64 seed = 0x18D6025F63DE8E0C;

    MtState mtState;

    InitializeState(&mtState, seed);

    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 12; j++) {
            zobristPieceHashes[i][j] = RandomUint64(&mtState);
        }
    }
    zobristBlacksTurn = RandomUint64(&mtState);


    for (int i = 0; i < 16; i++) {
        zobristCastlingRights[i] = RandomUint64(&mtState);
    }

    for (int i = 0; i < 8; i++) {
        zobristEnpassant[i] = RandomUint64(&mtState);
    }
}
