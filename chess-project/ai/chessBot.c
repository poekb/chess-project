#include "chessBot.h"
#include <stdlib.h>
#include "../logic/evaluator.h"
#include "moveOrdering.h"

#include "../core/board.h"
#include "../game/notations.h"
#include "transpositionTable.h"

Board* board;

int transpositionCount;

int SearchAttacks(int alpha, int beta);

bool botQuit = true;

Move movesBuffer[5000]; // static move buffer for fewer mallocs
int movesBufferOffset = 0;

Move bestMove;

Move bestMoveThisIteration;
int bestEval;

/*
    Main Search function
    Implements the minimax algorith with alpha-beta pruning
*/
int Search(int depth, int distFromRoot, int alpha, int beta) {
    if (botQuit) {
        return 0;
    }

    if (board->currentGameState.halfmoveClock >= (distFromRoot == 0) ? 6 : 4) {
        int count = 0;
        for (int i = board->gameStateHistoryCount - board->currentGameState.halfmoveClock;
             i < board->gameStateHistoryCount; i += 2) {
            if (board->zobristHistory[i] == board->zobristHash) {
                count++;
                if (count > (distFromRoot == 0) ? 1 : 0)
                    return 0; // Evaluate as draw if it is a repetition
            }
        }
    }

    int transposEval = GetTransposition(board->zobristHash, distFromRoot, depth, alpha, beta);
    if (transposEval != TranspositionNotFound) {
        transpositionCount++;

        Move transposMove = GetBestMoveFromTranspos(board->zobristHash);
        bool isRepetition = false;

        if (board->currentGameState.halfmoveClock >= 3) {
            Uint64 zobrist = GetZobistOfMove(board, transposMove);
            for (int i = board->gameStateHistoryCount - board->currentGameState.halfmoveClock;
                 i < board->gameStateHistoryCount; i += 2) {
                if (board->zobristHistory[i] == zobrist)
                    isRepetition = true; // Evaluate as draw if it is a repetition
            }
        }
        if (!isRepetition) {
            if (distFromRoot == 0) {
                bestMoveThisIteration = transposMove;
            }
            return transposEval;
        }
    }

    if (depth == 0) {
        return SearchAttacks(alpha, beta);
    }

    int moveCount = GenerateMoves(board, &movesBuffer[movesBufferOffset], false);

    if (moveCount == 0) {
        if ((((Uint64) 1 << board->kingSquare[board->isWhitesMove ? WhiteIndex : BlackIndex]) &
             board->underAttackMap) != 0) {
            int mateScore = MateScore - distFromRoot; // higher eval for closest mate
            return -mateScore;
        }
        return 0;
    }

    OrderMoves(board, &movesBuffer[movesBufferOffset], moveCount);


    Move bestMoveThisPos = 0;

    Uint8 transEvalType = TranspositionUpper;

    for (int i = 0; i < moveCount; i++) {
        MakeMove(board, movesBuffer[movesBufferOffset + i]);

        movesBufferOffset += moveCount;
        int eval = -Search(depth - 1, distFromRoot + 1, -beta, -alpha);
        movesBufferOffset -= moveCount;

        UnmakeMove(board, movesBuffer[movesBufferOffset + i]);

        if (eval >= beta) {
            InsertTransposition(board->zobristHash, beta, distFromRoot, depth, TranspositionLower,
                                movesBuffer[movesBufferOffset + i]);
            return beta;
        }

        if (botQuit) {
            return 0;
        }

        if (eval > alpha) {
            bestMoveThisPos = movesBuffer[movesBufferOffset + i];
            alpha = eval;
            transEvalType = TranspositionExact;
            if (distFromRoot == 0) {
                bestMoveThisIteration = movesBuffer[movesBufferOffset + i];
            }
        }
    }

    InsertTransposition(board->zobristHash, alpha, distFromRoot, depth, transEvalType, bestMoveThisPos);

    return alpha;
}

// Search with iterative deepening (can be terminated, by setting the botQuit global value to true)
Move FindBestMove(Board* boardIn) {
    board = boardIn;
    transpositionCount = 0;
    bestMove = 0;
    bestMoveThisIteration = 0;

    int depth = 1;
    int eval = 0;
    while (!botQuit && depth < 20) {
        bestMoveThisIteration = 0;
        eval = Search(depth, 0, -MateScore, MateScore);

        if (botQuit)
            break;
        depth++;

        bestMove = bestMoveThisIteration;
        bestEval = eval;
        if (IsMateEval(eval) && eval > 0)
            break;
    }
    printf("depth: %d\n", depth);
    printf("Eval: %10d\n", bestEval);

    return bestMove;
}

// Start the Search and terminate it after given time
Move StartBot(Board* board) {
    botQuit = false;

    SDL_Thread* botThread = SDL_CreateThread((int (*)(void *)) FindBestMove, "bot", board);

    SDL_Delay(2000);
    botQuit = true;
    SDL_WaitThread(botThread, 0);

    char* note = GetMoveNotation(board, bestMove);
    free(note);

    return bestMove;
}

/*
    Secondary Search function
    Only searches capture moves
*/
int SearchAttacks(int alpha, int beta) {
    if (botQuit) {
        return 0;
    }
    int eval = EvalBoard(board);
    if (eval >= beta)
        return beta;
    alpha = max(alpha, eval);

    int moveCount = GenerateMoves(board, &movesBuffer[movesBufferOffset], true);
    OrderMoves(board, &movesBuffer[movesBufferOffset], moveCount);

    for (int i = 0; i < moveCount; i++) {
        MakeMove(board, movesBuffer[movesBufferOffset + i]);

        movesBufferOffset += moveCount;
        int eval = -SearchAttacks(-beta, -alpha);
        movesBufferOffset -= moveCount;

        UnmakeMove(board, movesBuffer[movesBufferOffset + i]);
        if (eval >= beta) {
            return beta;
        }
        alpha = max(alpha, eval);
    }
    return alpha;
}

bool IsMateEval(int eval) {
    if (abs(eval) > MateScore + 1)
        return false;

    return (abs(eval) > MateScore - 999);
}
