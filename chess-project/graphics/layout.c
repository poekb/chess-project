#include "layout.h"
#include "../game/gameHandler.h"
#include "boardRenderer.h"
#include "textRenderer.h"

#define GAP_TOP 0.08
#define GAP_LEFT 0.01
#define GAP_RIGHT 0.3
#define MOVE_BUTTON_SIZE 0.7

// Button and textbox rectangles
SDL_Rect nextMoveRect;
SDL_Rect prevMoveRect;
SDL_Rect firstMoveRect;
SDL_Rect lastMoveRect;

SDL_Rect pasteFENRect;
SDL_Rect pastePGNRect;

SDL_Rect copyFENRect;
SDL_Rect copyPGNRect;

SDL_Rect savePGNRect;
SDL_Rect loadPGNRect;

SDL_Rect resetRect;

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

ButtonData lastMoveButton = {
    ">>",
    &nextEnabled,
    &lastMove,
    0xF0DADAFF,
    0x303040FF,
    0x101020FF,
    0x505060FF,
    &lastMoveRect
};

ButtonData firstMoveButton = {
    "<<",
    &prevEnabled,
    &firstMove,
    0xF0DADAFF,
    0x303040FF,
    0x101020FF,
    0x505060FF,
    &firstMoveRect
};


ButtonData pasteFENButton = {
    "Paste FEN",
    &gameLoadEnabled,
    &pasteFEN,
    0xF0DADAFF,
    0x303040FF,
    0x101020FF,
    0x505060FF,
    &pasteFENRect
};

ButtonData pastePGNButton = {
    "Paste PGN",
    &gameLoadEnabled,
    &pastePGN,
    0xF0DADAFF,
    0x303040FF,
    0x101020FF,
    0x505060FF,
    &pastePGNRect
};

ButtonData copyFENButton = {
    "Copy FEN",
    &gameLoadEnabled,
    &copyFEN,
    0xF0DADAFF,
    0x303040FF,
    0x101020FF,
    0x505060FF,
    &copyFENRect
};

ButtonData copyPGNButton = {
    "Copy PGN",
    &gameLoadEnabled,
    &copyPGN,
    0xF0DADAFF,
    0x303040FF,
    0x101020FF,
    0x505060FF,
    &copyPGNRect
};

ButtonData resetButton = {
    "Reset Board",
    &gameLoadEnabled,
    &resetBoard,
    0xF0DADAFF,
    0x303040FF,
    0x101020FF,
    0x505060FF,
    &resetRect
};

ButtonData saveButton = {
    "Save PGN",
    &gameLoadEnabled,
    &savePGN,
    0xF0DADAFF,
    0x303040FF,
    0x101020FF,
    0x505060FF,
    &savePGNRect
};

ButtonData loadButton = {
    "Load PGN",
    &gameLoadEnabled,
    &loadPGN,
    0xF0DADAFF,
    0x303040FF,
    0x101020FF,
    0x505060FF,
    &loadPGNRect
};


#define ButtonCount 11
ButtonData* buttons[ButtonCount] = {
    &nextMoveButton, &prevMoveButton,&firstMoveButton, &lastMoveButton, &pasteFENButton, &pastePGNButton, &copyFENButton, &copyPGNButton, &resetButton, &saveButton, &loadButton
};

UIData UI = { 0 };

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

    // Calculating the positions of the buttons
    lastMoveRect = (SDL_Rect){
        UI.gapLeft + UI.boardSize - moveButtonSize,
        boardY + UI.boardSize + (UI.gapTop - moveButtonSize) / 2,
        moveButtonSize,
        moveButtonSize
    };

    nextMoveRect = (SDL_Rect){
        UI.gapLeft + UI.boardSize - 2 * moveButtonSize - (UI.gapTop - moveButtonSize) / 2,
        boardY + UI.boardSize + (UI.gapTop - moveButtonSize) / 2,
        moveButtonSize,
        moveButtonSize
    };

    prevMoveRect = (SDL_Rect){
        UI.gapLeft + UI.boardSize - 3 * moveButtonSize - 2 * (UI.gapTop - moveButtonSize) / 2,
        boardY + UI.boardSize + (UI.gapTop - moveButtonSize) / 2,
        moveButtonSize,
        moveButtonSize
    };

    firstMoveRect = (SDL_Rect){
        UI.gapLeft + UI.boardSize - 4 * moveButtonSize - 3 * (UI.gapTop - moveButtonSize) / 2,
        boardY + UI.boardSize + (UI.gapTop - moveButtonSize) / 2,
        moveButtonSize,
        moveButtonSize
    };


    pasteFENRect = (SDL_Rect){
        UI.windowWidth - UI.gapLeft + moveButtonSize - UI.gapRight,
        boardY,
        UI.gapRight - moveButtonSize,
        UI.gapRight / 8
    };

    pastePGNRect = (SDL_Rect){
        UI.windowWidth - UI.gapLeft + moveButtonSize - UI.gapRight,
        boardY + UI.gapRight / 6 * 1,
        UI.gapRight - moveButtonSize,
        UI.gapRight / 8
    };

    copyFENRect = (SDL_Rect){
        UI.windowWidth - UI.gapLeft + moveButtonSize - UI.gapRight,
        boardY + UI.gapRight / 6 * 2,
        UI.gapRight - moveButtonSize,
        UI.gapRight / 8
    };

    copyPGNRect = (SDL_Rect){
        UI.windowWidth - UI.gapLeft + moveButtonSize - UI.gapRight,
        boardY + UI.gapRight / 6 * 3,
        UI.gapRight - moveButtonSize,
        UI.gapRight / 8
    };

    savePGNRect = (SDL_Rect){
        UI.windowWidth - UI.gapLeft + moveButtonSize - UI.gapRight,
        boardY + UI.gapRight / 6 * 4,
        UI.gapRight - moveButtonSize,
        UI.gapRight / 8
    };

    loadPGNRect = (SDL_Rect){
        UI.windowWidth - UI.gapLeft + moveButtonSize - UI.gapRight,
        boardY + UI.gapRight / 6 * 5,
        UI.gapRight - moveButtonSize,
        UI.gapRight / 8
    };



    resetRect = (SDL_Rect){
        UI.windowWidth - UI.gapLeft + moveButtonSize - UI.gapRight,
        boardY + UI.boardSize - UI.gapRight / 8,
        UI.gapRight - moveButtonSize,
        UI.gapRight / 8
    };
}

void renderButtons() {
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
}

void testButonClick() {
    for (int i = 0; i < ButtonCount; i++) {
        ButtonData* button = buttons[i];

        bool hower = mouseX > button->rect->x && mouseX < button->rect->x + button->rect->w && mouseY > button->rect->y && mouseY < button->rect->y + button->rect->h;
        if (hower && (*button->enabled))
            (*button->callBack)();
    }
}


SDL_Color HexToRGBA(int hex) {
    return (SDL_Color) { hex >> 24, hex >> 16 & 0xFF, hex >> 8 & 0xFF, hex & 0xFF };
}