#include "../global.h"

#ifndef UIDEFS
#define UIDEFS

typedef struct UIData {
    int windowWidth;
    int windowHeight;
    int boardSize;
    int gapTop;
    int gapRight;
    int gapLeft;
} UIData;

typedef struct ButtonData {
    char text[50];
    bool* enabled;
    void(*callBack)();
    int textColor;
    int defaultColor;
    int disabledColor;
    int howerColor;
    SDL_Rect* rect;
} ButtonData;

#endif

void renderButtons();
void recalcUIData();
void testButonClick();
SDL_Color HexToRGBA(int hex);