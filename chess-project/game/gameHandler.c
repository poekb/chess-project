#include "gameHandler.h"

#include "../graphics/board-renderer.h"
#include "../graphics/text-renderer.h"

#include "../core/board.h"

#include "../moveGeneration/precompution.h"
#include "../moveGeneration/moveGenerator.h"

#include "../ai/chess-bot.h"

#include "../logic/evaluator.h"

#define GAP_TOP 0.08
#define GAP_LEFT 0.01
#define GAP_RIGHT 0.3
#define MOVE_BUTTON_SIZE 0.7

void nextMove();
void prevMove();
void recalcUIData();

void UpdateBoard(Board* board);
int hasMove(Uint8 start, Uint8 target);
void stashMove(Move move);

void testButonClick();
SDL_Color HexToRGBA(int hex);

Uint8 selectedPos = -1;
Uint8 howerPos = -1;

Move validMoves[100];
Move move;
int moveCount = 0;

bool bot = false;

UIData UI = { 0 };

Sint32 mouseX, mouseY;

Board* board;
MoveList* moveHistory = NULL;
MoveList* moveFuture = NULL;


void updateLoop() {

    recalcUIData();

    board = calloc(1, sizeof(Board));
    if (board == NULL) {
        SDL_Log("Unable to allocate memory");
        exit(1);
    }
    LoadBoardFromFEN(board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
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

                UpdateBoard(board);
                selectedPos = -1;
            }
            else {
                selectedPos = howerPos;
                UpdateBoard(board);
            }


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

// Button and textbox rectangles
SDL_Rect nextMoveRect;
SDL_Rect prevMoveRect;
bool nextEnabled = false;
bool prevEnabled = false;
ButtonData nextMoveButton = {
    ">",
    &nextEnabled,
    &nextMove,
    0xF0DADAFF,
    0x303040FF,
    0x101020FF,
    0x505060FF,
    &nextMoveRect
};

ButtonData prevMoveButton = {
    "<",
    &prevEnabled,
    &prevMove,
    0xF0DADAFF,
    0x303040FF,
    0x101020FF,
    0x505060FF,
    &prevMoveRect
};

#define ButtonCount 2
ButtonData* buttons[ButtonCount] = {
    &nextMoveButton, &prevMoveButton
};


void recalcUIData() {

    SDL_GetWindowSize(window, &UI.windowWidth, &UI.windowHeight);

    UI.gapTop = (int)(UI.windowHeight * GAP_TOP);
    UI.gapLeft = (int)(UI.windowWidth * GAP_LEFT);

    UI.gapRight = (int)(UI.windowWidth * GAP_RIGHT);

    UI.boardSize = min(
        UI.windowHeight - 2 * UI.gapTop,
        UI.windowWidth - UI.gapLeft - UI.gapRight
    );

    int boardY = (UI.windowHeight - UI.boardSize) / 2;

    renderBoard(renderer, UI.boardSize, UI.gapLeft, boardY);

    int moveButtonSize = (int)(UI.gapTop * MOVE_BUTTON_SIZE);

    nextMoveRect = (SDL_Rect){
        UI.gapLeft + UI.boardSize - moveButtonSize,
        boardY + UI.boardSize + (UI.gapTop - moveButtonSize) / 2,
        moveButtonSize,
        moveButtonSize
    };

    prevMoveRect = (SDL_Rect){
        UI.gapLeft + UI.boardSize - 2 * moveButtonSize - (UI.gapTop - moveButtonSize) / 2,
        boardY + UI.boardSize + (UI.gapTop - moveButtonSize) / 2,
        moveButtonSize,
        moveButtonSize
    };

}

void testButonClick() {
    for (int i = 0; i < ButtonCount; i++) {
        ButtonData* button = buttons[i];

        bool hower = mouseX > button->rect->x && mouseX < button->rect->x + button->rect->w && mouseY > button->rect->y && mouseY < button->rect->y + button->rect->h;
        if (hower && (*button->enabled))
            (*button->callBack)();
    }
}

void stashMove(Move move) {
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

    while (moveFuture != NULL) {
        MoveList* current = moveFuture;
        moveFuture = moveFuture->next;
        free(current);
    }
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


void UpdateBoard(Board* board) {

    renderDynamic(renderer);

    for (int i = 0; i < ButtonCount; i++) {
        ButtonData* button = buttons[i];

        bool hower = mouseX > button->rect->x && mouseX < button->rect->x + button->rect->w && mouseY > button->rect->y && mouseY < button->rect->y + button->rect->h;

        SDL_Color color = HexToRGBA(button->defaultColor);
        if (!(*button->enabled)) {
            color = HexToRGBA(button->disabledColor);
        }
        else if (hower) {
            color = HexToRGBA(button->howerColor);
        }
        
        renderTextbox(renderer, button->text, *(button->rect), color, HexToRGBA(button->textColor));

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

SDL_Color HexToRGBA(int hex) {
    return (SDL_Color) {hex >> 24, hex >> 16 & 0xFF, hex >> 8 & 0xFF, hex & 0xFF};
}