#include "../global.h"
#include <SDL_ttf.h>

void textRendererInit();
void textRendererCleanup();
void renderText(SDL_Renderer* renderer, char* text, int size, int pos_x, int pos_y, SDL_Color color);
void renderChar(SDL_Renderer* renderer, char c, int size, int pos_x, int pos_y, SDL_Color color);
void renderTextbox(SDL_Renderer* renderer, char* text, SDL_Rect rect, SDL_Color bgColor, SDL_Color textColor);
