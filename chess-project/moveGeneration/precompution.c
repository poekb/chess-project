#include "precompution.h"
#include "../logic/evaluator.h"

// These values are ofsets in the 1D representation of the board
// Directions Up, Down, Right, Left, Upper left, Lower right, Upper right, Lower left
int Directions[8] = {-8, 8, 1, -1, -9, 9, -7, 7};

Uint8 NumOfSquaresFromEdge[64][8];

int knightJumpPattern[8] = {15, 17, -17, -15, 10, -6, 6, -10};
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

Uint8 whitePawnAttacks[64][2];
Uint8 whitePawnAttacksCount[64];

Uint8 blackPawnAttacks[64][2];
Uint8 blackPawnAttacksCount[64];

Uint8 Uint8Min(Uint8 a, Uint8 b);

// Generate values that are used for the move generation and are static
void PreCompute() {
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            int N = y;
            int S = 7 - y;
            int E = 7 - x;
            int W = x;
            int index = y * 8 + x;
            NumOfSquaresFromEdge[index][0] = N;
            NumOfSquaresFromEdge[index][1] = S;
            NumOfSquaresFromEdge[index][2] = E;
            NumOfSquaresFromEdge[index][3] = W;
            NumOfSquaresFromEdge[index][4] = Uint8Min(N, W);
            NumOfSquaresFromEdge[index][5] = Uint8Min(S, E);
            NumOfSquaresFromEdge[index][6] = Uint8Min(N, E);
            NumOfSquaresFromEdge[index][7] = Uint8Min(S, W);

            // Knight moves
            knightMovesCount[index] = 0;
            knightAttackBitBoards[index] = 0;
            for (int i = 0; i < 8; i++) {
                int target = index + knightJumpPattern[i];

                int file = target % 8;

                if (0 <= target && target < 64 && abs(file - x) <= 2 /*Test if the move wraps around the board*/) {
                    knightMoves[index][knightMovesCount[index]++] = target;

                    knightAttackBitBoards[index] |= (Uint64) 1 << target;
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
                    kingAttackBitBoards[index] |= (Uint64) 1 << target;
                }
            }

            // Diagonal moves
            bishopMoves[index] = 0;
            queenMoves[index] = 0;
            for (int d = 4; d < 8; d++) {
                int current = index;

                for (int i = 0; i < NumOfSquaresFromEdge[index][d]; i++) {
                    current += Directions[d];

                    bishopMoves[index] |= (Uint64) 1 << current;
                    queenMoves[index] |= (Uint64) 1 << current;
                }
            }

            // Horizontal moves
            rookMoves[index] = 0;
            for (int d = 0; d < 4; d++) {
                int current = index;

                for (int i = 0; i < NumOfSquaresFromEdge[index][d]; i++) {
                    current += Directions[d];

                    rookMoves[index] |= (Uint64) 1 << current;
                    queenMoves[index] |= (Uint64) 1 << current;
                }
            }

            // Pawn moves
            pawnAttackBitBoards[index][0] = 0;
            pawnAttackBitBoards[index][1] = 0;

            if (x > 0) {
                if (y > 0) {
                    whitePawnAttacks[index][whitePawnAttacksCount[index]++] = index - 9;

                    pawnAttackBitBoards[index][0] |= (Uint64) 1 << (index - 9);
                }
                if (y < 7) {
                    blackPawnAttacks[index][blackPawnAttacksCount[index]++] = index + 7;
                    pawnAttackBitBoards[index][1] |= (Uint64) 1 << (index + 7);
                }
            }

            if (x < 7) {
                if (y > 0) {
                    whitePawnAttacks[index][whitePawnAttacksCount[index]++] = index - 7;
                    pawnAttackBitBoards[index][0] |= (Uint64) 1 << (index - 7);
                }
                if (y < 7) {
                    blackPawnAttacks[index][blackPawnAttacksCount[index]++] = index + 9;
                    pawnAttackBitBoards[index][1] |= (Uint64) 1 << (index + 9);
                }
            }
        }
    }
}

Uint8 Uint8Min(Uint8 a, Uint8 b) {
    return a < b ? a : b;
}
