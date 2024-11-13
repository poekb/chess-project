#include "../global.h"
#include "../core/board.h"

typedef struct ButtonData {
    char text[50];
    bool** enabled;
    void(*callBack)();
    int textColor;
    int defaultColor;
    int disabledColor;
    int howerColor;
    SDL_Rect* rect;
} ButtonData;

extern ButtonData toggleBotButton;

void renderButtons();
void renderButton(ButtonData* button);
void recalcUIData();
void testButonClicks();
void testButton(ButtonData* button);
void renderWinner(Board* board);

SDL_Color HexToRGBA(Uint32 hex);