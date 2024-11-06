#include "../global.h"

#include "../core/move.h"

#define TranspositionExact 0
#define TranspositionUpper 1
#define TranspositionLower 2
#define TranspositionNotFound MateScore + 999
typedef struct Transposition {
	Uint64 zobrist;
	int eval;
	Move bestMove;
	Uint8 depth;
	Uint8 type;
}Transposition;

extern Transposition* transpositionTable;

void initTranspositionTable();
void insertTransposition(Uint64 zobrist, int eval, Uint8 distFromRoot, Uint8 depth, Uint8 type, Move bestMove);
int getTransposition(Uint64 zobrist, Uint8 distFromRoot, Uint8 depth, int alpha, int beta);

Move getBestMoveFromTranspos(Uint64 zobrist);