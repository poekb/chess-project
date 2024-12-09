#pragma once
#include "../global.h"
#include "../core/board.h"

typedef struct ButtonData {
    char text[50];
    bool** enabled;

    void (*callBack)();

    int textColor;
    int defaultColor;
    int disabledColor;
    int howerColor;
    SDL_Rect* rect;
} ButtonData;

extern ButtonData toggleBotButton;

void RenderButtons();

void RenderButton(ButtonData* button);

void RecalcUIData();

void TestButonClicks();

void TestButton(ButtonData* button);

void RenderWinner(Board* board);

SDL_Color HexToRGBA(Uint32 hex);
