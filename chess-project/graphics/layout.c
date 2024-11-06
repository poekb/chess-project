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

// Data for every button (the unique colors may be unecassary because they are all the same anyway)

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


// The array of button settings
#define ButtonCount 11
ButtonData* buttons[ButtonCount] = {
    &nextMoveButton, &prevMoveButton,&firstMoveButton, &lastMoveButton, 
    &pasteFENButton, &pastePGNButton, 
    &copyFENButton, &copyPGNButton, 
    &resetButton, 
    &saveButton, &loadButton
};

// Calculate the positions of 
void recalcUIData() {
    int windowWidth;
    int windowHeight;

    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    int gapTop = (int)(windowHeight * GAP_TOP);
    int gapLeft = (int)(windowWidth * GAP_LEFT);

    int gapRight = (int)(windowWidth * GAP_RIGHT);

    int boardSize = min(
        windowHeight - 2 * gapTop,
        windowWidth - gapLeft - gapRight
    );

    int boardY = (windowHeight - boardSize) / 2;

    renderBoard(renderer, boardSize, gapLeft, boardY);

    int moveButtonSize = (int)(gapTop * MOVE_BUTTON_SIZE);

    // Calculating the positions of the buttons ?? Somehow make this cleaner ??

    SDL_Rect defaultMoveButtonRect = (SDL_Rect){
        gapLeft + boardSize - moveButtonSize,
        boardY + boardSize + (gapTop - moveButtonSize) / 2,
        moveButtonSize,
        moveButtonSize
    };
    int moveButtonOffsetValue = -moveButtonSize - (gapTop - moveButtonSize) / 2;

    lastMoveRect = defaultMoveButtonRect;

    nextMoveRect = defaultMoveButtonRect;
    nextMoveRect.x += moveButtonOffsetValue;


    prevMoveRect = defaultMoveButtonRect;
    prevMoveRect.x += moveButtonOffsetValue * 2;

    firstMoveRect = defaultMoveButtonRect;
    firstMoveRect.x += moveButtonOffsetValue * 3;

    SDL_Rect defaultMenuButtonRect = (SDL_Rect){
        windowWidth - gapLeft + moveButtonSize - gapRight,
        boardY,
        gapRight - moveButtonSize,
        gapRight / 8
    };
    int menuButtonOffsetValue = gapRight / 6;

    pasteFENRect = defaultMenuButtonRect;

    pastePGNRect = defaultMenuButtonRect;
    pastePGNRect.y += menuButtonOffsetValue;

    copyFENRect = defaultMenuButtonRect;
    copyFENRect.y += menuButtonOffsetValue * 2;

    copyPGNRect = defaultMenuButtonRect;
    copyPGNRect.y += menuButtonOffsetValue * 3;

    savePGNRect = defaultMenuButtonRect;
    savePGNRect.y += menuButtonOffsetValue * 4;

    loadPGNRect = defaultMenuButtonRect;
    loadPGNRect.y += menuButtonOffsetValue * 5;

    resetRect = defaultMenuButtonRect;
    resetRect.y += menuButtonOffsetValue * 6;
}

void renderButtons() {
    for (int i = 0; i < ButtonCount; i++) {
        ButtonData* button = buttons[i];

        renderButton(button);
    }
}

void renderButton(ButtonData* button) {
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

void testButonClicks() {
    for (int i = 0; i < ButtonCount; i++) {
        ButtonData* button = buttons[i];

        testButton(button);
    }
}

void testButton(ButtonData* button) {
    bool hower = mouseX > button->rect->x && mouseX < button->rect->x + button->rect->w && mouseY > button->rect->y && mouseY < button->rect->y + button->rect->h;
    if (hower && (*button->enabled))
        (*button->callBack)();
}

SDL_Color HexToRGBA(int hex) {
    return (SDL_Color) { hex >> 24, hex >> 16 & 0xFF, hex >> 8 & 0xFF, hex & 0xFF };
}