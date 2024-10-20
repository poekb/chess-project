#include "../global.h"
#include "../core/move.h"

void updateLoop();

#ifndef GAME
#define GAME
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

typedef struct MoveList {
    Move move;
    struct MoveList* next;
}MoveList;
#endif

