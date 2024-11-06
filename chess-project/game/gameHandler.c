#include "../ai/chessBot.h"
#include "../ai/transpositionTable.h"
#include "../core/board.h"
#include "../graphics/boardRenderer.h"
#include "../graphics/layout.h"
#include "../graphics/textRenderer.h"
#include "../moveGeneration/moveGenerator.h"
#include "gameHandler.h"
#include "notations.h"
#include "pgn.h"

void recalcUIData();

void UpdateBoard(Board* board);
int hasMove(Uint8 start, Uint8 target);
int countMoves(Board* board, int depth);

void freeMoveHistory();
void freeMoveFuture();

// These values are static:

Uint8 selectedPos = -1;
Uint8 howerPos = -1;

Move validMoves[100];
Move move;
int moveCount = 0;

bool bot = false;

Sint32 mouseX, mouseY;

char startFEN[100] = DefaultBoardFEN;
Board* board;
MoveList* moveHistory = NULL;
MoveList* moveFuture = NULL;

// Perft test for validating move generation
// Unused in the final app
void perftTest(int depth);

// The main update loop of the program
void updateLoop() {

    recalcUIData();
    loadFEN(startFEN);
    initTranspositionTable();

    // ############################
    // #   Main game event loop   #
    // ############################

    bool quit = false;
    SDL_Event event;
    while (!quit) {
        SDL_WaitEvent(&event);
        switch (event.type) {
        case SDL_KEYDOWN:

            bot = !bot;
            if (bot) {
                Move bestMove = findBestMove(board);
                if (bestMove != 0)
                    stashMove(bestMove);
            }
            renderStatic(renderer);
            UpdateBoard(board);

            break;
        case SDL_MOUSEMOTION:
            mouseX = event.motion.x;
            mouseY = event.motion.y;
            howerPos = getCell(event.motion.x, event.motion.y);

            UpdateBoard(board);

            break;
        case SDL_MOUSEBUTTONDOWN: {

            testButonClicks();

            int moveIndex = hasMove(selectedPos, howerPos);
            if (selectedPos != -1 && (moveIndex != -1)) {
                move = validMoves[moveIndex];
                stashMove(move);

                renderStatic(renderer);
                renderPieces(renderer, board->square);

                UpdateBoard(board);

                if (bot) {
                    Move bestMove = findBestMove(board);
                    if(bestMove != 0)
                        stashMove(bestMove);
                }

                selectedPos = -1;
            }
            else {
                selectedPos = howerPos;
            }
            UpdateBoard(board);

            break;
        }
        case SDL_WINDOWEVENT: {
            recalcUIData();
            UpdateBoard(board);

            break;
        }
        case SDL_QUIT:
            quit = true;
            break;
        }
    }

    free(board);
    while (moveFuture != NULL) {
        MoveList* current = moveFuture;
        moveFuture = moveFuture->next;
        free(current);
    }
    while (moveHistory != NULL) {
        MoveList* current = moveHistory;
        moveHistory = moveHistory->next;
        free(current);
    }

    free(transpositionTable);
}

bool nextEnabled = false;
bool prevEnabled = false;

bool gameLoadEnabled = true;

// Make a move and store it in move history
void stashMove(Move move) {

    makeMove(board,move);
    MoveList* moveList = malloc(sizeof(MoveList));
    if (moveList == NULL) {
        SDL_Log("Unable to allocate memory");
        exit(1);
    }
    moveList->move = move;

    moveList->next = moveHistory;

    moveHistory = moveList;

    nextEnabled = false;
    prevEnabled = true;

    freeMoveFuture();
}

// Load move from move future
void nextMove() {

    makeMove(board, moveFuture->move);
    MoveList* current = moveFuture;
    moveFuture = moveFuture->next;
    current->next = moveHistory;
    moveHistory = current;
    prevEnabled = true;

    if (moveFuture == NULL) {
        nextEnabled = false;
    }
}

// Load history from move history and store the revoked move in move future
void prevMove() {

    unmakeMove(board, moveHistory->move);
    MoveList* current = moveHistory;
    moveHistory = moveHistory->next;
    current->next = moveFuture;
    moveFuture = current;
    nextEnabled = true;

    if (moveHistory == NULL) {
        prevEnabled = false;
    }
}

// Go back to the starting position of the game, but store the moves made
void firstMove() {
    while (prevEnabled) {
        prevMove();
    }
}

// Go to the last future move
void lastMove() {
    while (nextEnabled) {
        nextMove();
    }
}

void freeMoveHistory() {
    while (moveHistory != NULL) {
        MoveList* current = moveHistory;
        moveHistory = moveHistory->next;
        free(current);
    }
}

void freeMoveFuture() {
    while (moveFuture != NULL) {
        MoveList* current = moveFuture;
        moveFuture = moveFuture->next;
        free(current);
    }
}

//
void UpdateBoard(Board* board) {

    renderStatic(renderer);

    renderButtons();

    if (prevEnabled) {
        highlightCell(renderer, getStart(moveHistory->move), (SDL_Color) { 70, 200, 70, 200 });
        highlightCell(renderer, getTarget(moveHistory->move), (SDL_Color) { 130, 230, 130, 200 });
    }

    if (selectedPos != -1) {
        highlightCell(renderer, selectedPos, (SDL_Color) { 100, 100, 200, 150 });
    }

    if ((howerPos != -1))
        highlightCell(renderer, howerPos, (SDL_Color) { 100, 100, 200, 150 });

    moveCount = generateMoves(board, validMoves, false);

    for (int i = 0; i < moveCount; i++) {
        Uint8 start = getStart(validMoves[i]);
        Uint8 target = getTarget(validMoves[i]);

        if (start == selectedPos) {
            highlightCell(renderer, target, (SDL_Color) { 100, 100, 200, 150 });
        }
    }

    renderPieces(renderer, board->square);
    SDL_RenderPresent(renderer);
}

void resetBoard() {
    loadFEN(DefaultBoardFEN);
}

void pasteFEN() {
    char* clipBoard = SDL_GetClipboardText();

    loadFEN(clipBoard);
    SDL_free(clipBoard);
}

void pastePGN() {
    char* clipBoard = SDL_GetClipboardText();

    loadGameFromPGN(board, clipBoard);
    SDL_free(clipBoard);
}

void copyFEN() {
    char FEN[100];
    getFENFromBoard(board, FEN);
    SDL_SetClipboardText(FEN);
}

void copyPGN() {
    char* PGN = getPGN(board);
    SDL_SetClipboardText(PGN);
    free(PGN);
}

void savePGN() {
    char* PGN = getPGN(board);

    FILE* fptr = fopen("game.pgn", "w");

    fprintf(fptr, "%s\n", PGN);

    fclose(fptr);
    free(PGN);
}

void loadPGN() {
    FILE* fptr = fopen("game.pgn", "r");
    if (fptr == NULL) return;

    fseek(fptr, 0L, SEEK_END);
    size_t size = ftell(fptr);
    rewind(fptr);

    char* PGN = calloc(size + 1, sizeof(char));

    if (PGN == NULL) return;

    fread(PGN, sizeof(char), size, fptr);
    fclose(fptr);

    loadGameFromPGN(board, PGN);
    free(PGN);
}

void loadFEN(char* fenStr) {
    memcpy(startFEN, fenStr, min(strlen(fenStr) + 1,100));
    startFEN[99] = '\0';

    freeMoveFuture();
    freeMoveHistory();
    prevEnabled = false;
    nextEnabled = false;

    if (board != NULL) free(board);

    board = calloc(1, sizeof(Board));
    if (board == NULL) {
        SDL_Log("Unable to allocate memory");
        exit(1);
    }

    LoadBoardFromFEN(board, fenStr);

    // Optional perft test for validating the move generator:
    perftTest(5);
}

// For move generator validation
void perftTest(int depth) {
    Uint64 oldTick = SDL_GetTicks64();
    for (int i = 1; i <= depth; i++) {
        printf("Num of possible moves (%d): %10d ", i, countMoves(board, i));
        printf("in %lu ms\n", (int)(SDL_GetTicks64() - oldTick));
    }
}
int countMoves(Board* board, int depth) {
    if (depth == 0) return 1;

    int sum = 0; // temp 1
    Move* moves = malloc(sizeof(Move) * 100);
    if (moves == NULL) return 1;
    int moveCount = generateMoves(board, moves, false);

    for (int i = 0; i < moveCount; i++) {
        makeMove(board, moves[i]);
        
        int count = countMoves(board, depth - 1);
        if (isPromotion(moves[i]))
            sum += count;
        else
            sum += count;
        unmakeMove(board, moves[i]);
    }
    free(moves);
    return max(sum, 0);
}

// Check if the move is part of the valid moves
int hasMove(Uint8 start, Uint8 target) {
    for (int i = 0; i < moveCount; i++) {
        Uint8 s = getStart(validMoves[i]);
        Uint8 t = getTarget(validMoves[i]);

        if (s == start && t == target) {

            return i;
        }
    }
    return -1;
}

// Return the next move in move future, this is important for PGN generation
Move getNextMove() {
    return moveFuture->move;
}
