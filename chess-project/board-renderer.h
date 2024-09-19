#include "global.h"

#include <SDL_ttf.h>
#include <SDL_image.h>

void rendererInit(SDL_Renderer* renderer);
void rederer_cleanUp();
void renderBoard(SDL_Renderer* renderer, double tableSize, Uint8 board[8][8]);
void renderDynamic(SDL_Renderer* renderer);
void highlightCell(SDL_Renderer* renderer, Pos pos);
void displayEval(SDL_Renderer* renderer, PossibleMove* move);

void sdlInit(int szeles, int magas, SDL_Window** pwindow, SDL_Renderer** prenderer);