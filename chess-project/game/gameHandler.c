#include "gameHandler.h"
#include "../graphics/board-renderer.h"
#include "../graphics/text-renderer.h"
#include "../core/board.h"
#include "../moveGeneration/moveGenerator.h"
#include "../ai/chess-bot.h"
#include "notations.h"
#include "../graphics/layout.h"
#include "pgn.h"

void recalcUIData();

void UpdateBoard(Board* board);
int hasMove(Uint8 start, Uint8 target);
void stashMove(Move move);

void loadFEN(char* fenStr);

void freeMoveHistory();
void freeMoveFuture();

Uint8 selectedPos = -1;
Uint8 howerPos = -1;

Move validMoves[100];
Move move;
int moveCount = 0;

bool bot = false;

Sint32 mouseX, mouseY;

char startFEN[100] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
Board* board;
MoveList* moveHistory = NULL;
MoveList* moveFuture = NULL;


void updateLoop() {

    recalcUIData();

    loadFEN(startFEN);
    //LoadBoardFromFEN(board, "7k/1p4p1/p4b1p/3N3P/2p5/2rb4/PP2r3/K2R2R1 b - - 0 1");


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
                Move bestMove = CalcBestMove(board);
                if (bestMove != 0)
                    stashMove(bestMove);
            }
           
            //CalcBestMove(board);
            renderDynamic(renderer);
            UpdateBoard(board);

            break;
        case SDL_MOUSEMOTION:
            mouseX = event.motion.x;
            mouseY = event.motion.y;
            howerPos = getCell(event.motion.x, event.motion.y);

            UpdateBoard(board);

            break;
        case SDL_MOUSEBUTTONDOWN: {

            testButonClick();

            int moveIndex = hasMove(selectedPos, howerPos);
            if (selectedPos != -1 && (moveIndex != -1)) {
                move = validMoves[moveIndex];
                stashMove(move);

                renderDynamic(renderer);
                renderPieces(renderer, board->square);

                UpdateBoard(board);

                if (bot) {
                    Move bestMove = CalcBestMove(board);
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
}

bool nextEnabled = false;
bool prevEnabled = false;

bool gameLoadEnabled = true;

void stashMove(Move move) {

    char* notationStr = getMoveNotation(board, move);
    printf("%s\n", notationStr);

    MakeMove(board,move);
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

void nextMove() {

    MakeMove(board, moveFuture->move);
    MoveList* current = moveFuture;
    moveFuture = moveFuture->next;
    current->next = moveHistory;
    moveHistory = current;
    prevEnabled = true;

    if (moveFuture == NULL) {
        nextEnabled = false;
    }
}

void prevMove() {

    RevokeMove(board, moveHistory->move);
    MoveList* current = moveHistory;
    moveHistory = moveHistory->next;
    current->next = moveFuture;
    moveFuture = current;
    nextEnabled = true;

    if (moveHistory == NULL) {
        prevEnabled = false;
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

void UpdateBoard(Board* board) {

    renderDynamic(renderer);

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

    /*displayBitboard(renderer, board->underAttackMap, (SDL_Color) { 200, 100, 100, 150 });
    highlightCells(renderer, &board->kingSquare[board->isWhitesMove ? WhiteIndex : BlackIndex], 1, (SDL_Color) { 100, 200, 100, 150 });*/
    renderPieces(renderer, board->square);
    SDL_RenderPresent(renderer);
}

void resetBoard() {
    loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

void pasteFEN() {
    char* clipBoard = SDL_GetClipboardText();

    printf("%s\n",clipBoard);
    loadFEN(clipBoard);

    SDL_free(clipBoard);
}

void copyFEN() {
    char FEN[100];
    getFENFromBoard(board, FEN);
    SDL_SetClipboardText(FEN);
}

void copyPGN() {
    char* pgn = getPGN(board);
    printf("%s\n", pgn);
    free(pgn);
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
}

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

Move getNextMove() {
    return moveFuture->move;
}
