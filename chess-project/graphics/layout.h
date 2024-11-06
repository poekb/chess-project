#include "../global.h"

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

void renderButtons();
void renderButton(ButtonData* button);
void recalcUIData();
void testButonClicks();
void testButton(ButtonData* button);
SDL_Color HexToRGBA(int hex);