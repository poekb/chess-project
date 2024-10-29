#include "precompution.h"
#include "../logic/evaluator.h"
// Direction N, S, E, W, NW, SE, NE, SW
int Directions[8] = { -8, 8, 1, -1, -9, 9, -7, 7};

Uint8 NumSquaresFromEdge[64][8];

int allKnightJumps[8] = { 15, 17, -17, -15, 10, -6, 6, -10 };
Uint8 knightMoves[64][8];
Uint8 knightMovesCount[64];

Uint8 kingMoves[64][8];
Uint8 kingMovesCount[64];

// Bitboards
Uint64 queenMoves[64];
Uint64 bishopMoves[64];
Uint64 rookMoves[64];

Uint64 knightAttackBitBoards[64];
Uint64 kingAttackBitBoards[64];
Uint64 pawnAttackBitBoards[64][2];


Uint8 pawnAttacks[64][2][2];
Uint8 pawnAttacksCount[64][2];

Uint8 whitePawnAttacks[64][2];
Uint8 whitePawnAttacksCount[64];

Uint8 blackPawnAttacks[64][2];
Uint8 blackPawnAttacksCount[64];


Uint8 Uint8Min(Uint8 a, Uint8 b) {
	return a < b ? a : b;
}

void preCompute() {
	
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			int N = y;
			int S = 7 - y;
			int E = 7 - x;
			int W = x;
			int index = y * 8 + x;
			NumSquaresFromEdge[index][0] = N;
			NumSquaresFromEdge[index][1] = S;
			NumSquaresFromEdge[index][2] = E;
			NumSquaresFromEdge[index][3] = W;
			NumSquaresFromEdge[index][4] = Uint8Min(N, W);
			NumSquaresFromEdge[index][5] = Uint8Min(S, E);
			NumSquaresFromEdge[index][6] = Uint8Min(N, E);
			NumSquaresFromEdge[index][7] = Uint8Min(S, W);

			// Knight moves
			knightMovesCount[index] = 0;
			knightAttackBitBoards[index] = 0;
			for (int i = 0; i < 8; i++) {
				int target = index + allKnightJumps[i];

				int tx = target % 8;
				
				if (0 <= target && target < 64 && abs(tx - x) <= 2) {
					knightMoves[index][knightMovesCount[index]++] = target;

					knightAttackBitBoards[index] |= (Uint64)1 << target;
				}
			}

			// King moves
			kingMovesCount[index] = 0;
			kingAttackBitBoards[index] = 0;
			for (int i = 0; i < 8; i++) {
				int target = index + Directions[i];

				int tx = target % 8;

				if (0 <= target && target < 64 && abs(tx - x) <= 1) {
					kingMoves[index][kingMovesCount[index]++] = target;
					kingAttackBitBoards[index] |= (Uint64)1 << target;
				}
			}

			// Diagonal moves
			bishopMoves[index] = 0;
			queenMoves[index] = 0;
			for (int d = 4; d < 8; d++) {
				int current = index;

				for (int i = 0; i < NumSquaresFromEdge[index][d]; i++) {

					current += Directions[d];

					bishopMoves[index] |= (Uint64)1 << current;
					queenMoves[index] |= (Uint64)1 << current;
				}
			}

			// Horizontal moves
			rookMoves[index] = 0;
			for (int d = 0; d < 4; d++) {
				int current = index;

				for (int i = 0; i < NumSquaresFromEdge[index][d]; i++) {

					current += Directions[d];

					rookMoves[index] |= (Uint64)1 << current;
					queenMoves[index] |= (Uint64)1 << current;
				}
			}

			// Pawn moves
			pawnAttackBitBoards[index][0] = 0;
			pawnAttackBitBoards[index][1] = 0;

			if (x > 0) {
				if (y > 0) {
					whitePawnAttacks[index][whitePawnAttacksCount[index]++] = index - 9;

					pawnAttackBitBoards[index][0] |= (Uint64)1 << (index - 9);
				}
				if (y < 7) {
					blackPawnAttacks[index][blackPawnAttacksCount[index]++] = index + 7;
					pawnAttackBitBoards[index][1] |= (Uint64)1 << (index + 7);

				}
			}

			if (x < 7) {
				if (y > 0) {
					whitePawnAttacks[index][whitePawnAttacksCount[index]++] = index - 7;
					pawnAttackBitBoards[index][0] |= (Uint64)1 << (index - 7);
				}
				if (y < 7) {
					blackPawnAttacks[index][blackPawnAttacksCount[index]++] = index + 9;
					pawnAttackBitBoards[index][1] |= (Uint64)1 << (index + 9);
				}
			}

		}
	}
}
