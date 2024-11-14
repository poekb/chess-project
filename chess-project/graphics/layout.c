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
SDL_Rect toggleBotButtonRect;
SDL_Rect winnerRect;


// Data for every button (the unique colors may be unecessary because they are all the same anyway)
ButtonData nextMoveButton = {
    ">",
    &nextEnabled,
    &NextMove,
    0xF0DADAFF,
    0x303040FF,
    0x101020FF,
    0x505060FF,
    &nextMoveRect
};

ButtonData prevMoveButton = {
    "<",
    &prevEnabled,
    &PrevMove,
    0xF0DADAFF,
    0x303040FF,
    0x101020FF,
    0x505060FF,
    &prevMoveRect
};

ButtonData lastMoveButton = {
    ">>",
    &nextEnabled,
    &LastMove,
    0xF0DADAFF,
    0x303040FF,
    0x101020FF,
    0x505060FF,
    &lastMoveRect
};

ButtonData firstMoveButton = {
    "<<",
    &prevEnabled,
    &FirstMove,
    0xF0DADAFF,
    0x303040FF,
    0x101020FF,
    0x505060FF,
    &firstMoveRect
};

ButtonData pasteFENButton = {
    "Paste FEN",
    &gameLoadEnabled,
    &PasteFEN,
    0xF0DADAFF,
    0x303040FF,
    0x101020FF,
    0x505060FF,
    &pasteFENRect
};

ButtonData pastePGNButton = {
    "Paste PGN",
    &gameLoadEnabled,
    &PastePGN,
    0xF0DADAFF,
    0x303040FF,
    0x101020FF,
    0x505060FF,
    &pastePGNRect
};

ButtonData copyFENButton = {
    "Copy FEN",
    &gameLoadEnabled,
    &CopyFEN,
    0xF0DADAFF,
    0x303040FF,
    0x101020FF,
    0x505060FF,
    &copyFENRect
};

ButtonData copyPGNButton = {
    "Copy PGN",
    &gameLoadEnabled,
    &CopyPGN,
    0xF0DADAFF,
    0x303040FF,
    0x101020FF,
    0x505060FF,
    &copyPGNRect
};

ButtonData resetButton = {
    "Reset Board",
    &gameLoadEnabled,
    &ResetBoard,
    0xF0DADAFF,
    0x303040FF,
    0x101020FF,
    0x505060FF,
    &resetRect
};

ButtonData saveButton = {
    "Save PGN",
    &gameLoadEnabled,
    &SavePGN,
    0xF0DADAFF,
    0x303040FF,
    0x101020FF,
    0x505060FF,
    &savePGNRect
};

ButtonData loadButton = {
    "Load PGN",
    &gameLoadEnabled,
    &LoadPGN,
    0xF0DADAFF,
    0x303040FF,
    0x101020FF,
    0x505060FF,
    &loadPGNRect
};


ButtonData toggleBotButton = {
    "Start Bot",
    NULL,
    &ToggleBot,
    0xF0DADAFF,
    0x303040FF,
    0x101020FF,
    0x505060FF,
    &toggleBotButtonRect
};

// The array of button settings
#define ButtonCount 12
ButtonData* buttons[ButtonCount] = {
    &nextMoveButton, &prevMoveButton,&firstMoveButton, &lastMoveButton, 
    &pasteFENButton, &pastePGNButton, 
    &copyFENButton, &copyPGNButton, 
    &resetButton, 
    &saveButton, &loadButton,
    &toggleBotButton
};

// Calculate the positions of UI elements
void RecalcUIData() {
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

    RenderBoard(renderer, boardSize, gapLeft, boardY);

    int moveButtonSize = min((int)(gapTop * MOVE_BUTTON_SIZE), gapRight / 8);

    int moveGap = min((gapTop - moveButtonSize) / 2, moveButtonSize / 4);

    SDL_Rect defaultMoveButtonRect = (SDL_Rect){
        gapLeft + boardSize - moveButtonSize,
        boardY + boardSize + moveGap,
        moveButtonSize,
        moveButtonSize
    };
    int moveButtonOffsetValue = -moveButtonSize - moveGap;

    lastMoveRect = defaultMoveButtonRect;

    nextMoveRect = defaultMoveButtonRect;
    nextMoveRect.x += moveButtonOffsetValue;


    prevMoveRect = defaultMoveButtonRect;
    prevMoveRect.x += moveButtonOffsetValue * 2;

    firstMoveRect = defaultMoveButtonRect;
    firstMoveRect.x += moveButtonOffsetValue * 3;

    

    toggleBotButtonRect = (SDL_Rect){
        gapLeft + boardSize - moveButtonSize + moveButtonOffsetValue * 3,
        boardY - moveGap - moveButtonSize,
        moveButtonSize - moveButtonOffsetValue * 3,
        moveButtonSize
    };

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
    copyFENRect.y += (int)(menuButtonOffsetValue * 2.5);

    copyPGNRect = defaultMenuButtonRect;
    copyPGNRect.y += (int)(menuButtonOffsetValue * 3.5);

    savePGNRect = defaultMenuButtonRect;
    savePGNRect.y += menuButtonOffsetValue * 5;

    loadPGNRect = defaultMenuButtonRect;
    loadPGNRect.y += menuButtonOffsetValue * 6;

    resetRect = defaultMoveButtonRect;
    resetRect.x = defaultMenuButtonRect.x;
    resetRect.w = defaultMenuButtonRect.w;

    winnerRect = (SDL_Rect){
        windowWidth - gapLeft + moveButtonSize - gapRight,
        boardY + boardSize - gapRight / 6,
        gapRight - moveButtonSize,
        gapRight / 6,
    };

}

// Display who won or draw
void RenderWinner(Board* board) {

    if (board->hasGameEnded) {
        if (board->winnerWhite) {
            RenderTextbox(renderer, "White Wins", winnerRect, HexToRGBA(COLOR_WHITE), HexToRGBA(COLOR_BACKGROUND));

        }
        else if (board->winnerBlack) {
            RenderTextbox(renderer, "Black Wins", winnerRect, HexToRGBA(COLOR_BLACK), HexToRGBA(COLOR_BACKGROUND));

        }
        else {
            RenderTextbox(renderer, "Draw", winnerRect, HexToRGBA(0x101020FF), HexToRGBA(0xF0DADAFF));
        }
    }
    else {
        RenderTextbox(renderer, " ", winnerRect, HexToRGBA(COLOR_BACKGROUND), HexToRGBA(COLOR_BACKGROUND));
    }
}

void RenderButtons() {
    for (int i = 0; i < ButtonCount; i++) {
        ButtonData* button = buttons[i];

        RenderButton(button);
    }
}

void RenderButton(ButtonData* button) {
    bool hower = mouseX > button->rect->x && mouseX < button->rect->x + button->rect->w && mouseY > button->rect->y && mouseY < button->rect->y + button->rect->h;

    SDL_Color color = HexToRGBA(button->defaultColor);
    if (!((button->enabled == NULL) || **button->enabled)) {
        color = HexToRGBA(button->disabledColor);
    }
    else if (hower) {
        color = HexToRGBA(button->howerColor);
    }

    RenderTextbox(renderer, button->text, *(button->rect), color, HexToRGBA(button->textColor));
}

void TestButonClicks() {
    for (int i = 0; i < ButtonCount; i++) {
        ButtonData* button = buttons[i];

        TestButton(button);
    }
}

void TestButton(ButtonData* button) {
    bool hower = mouseX > button->rect->x && mouseX < button->rect->x + button->rect->w && mouseY > button->rect->y && mouseY < button->rect->y + button->rect->h;
    if (hower && ((button->enabled == NULL) || (**button->enabled)))
        (*button->callBack)();
}

SDL_Color HexToRGBA(Uint32 hex) {
    return (SDL_Color) { hex >> 24, hex >> 16 & 0xFF, hex >> 8 & 0xFF, hex & 0xFF };
}