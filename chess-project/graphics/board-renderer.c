#include "../logic/move-eval.h"
#include "svg-loader.h"
#include "text-renderer.h"

#include "board-renderer.h"

const SDL_Color COLOR_BLACK = { 0x5a, 0x40, 0x30, 0xFF };
const SDL_Color COLOR_WHITE = { 0xFF, 0xF0, 0xD0, 0xFF };
const SDL_Color COLOR_HIGH = { 0x80, 0x80, 0x90, 0xa0 };


SDL_Texture* staticImgBuffer;

void setDrawColor(SDL_Renderer* renderer, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void rendererInit(SDL_Renderer* renderer) {

    textRend_init();
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SVG_init(renderer);
}

void rederer_cleanUp() {
    textRend_clean();
    SVG_clear();
}

double tableSize = 0;
double cellSize;

void renderBoard(SDL_Renderer* renderer, double tableSizeNew, Uint8 board[8][8]) {
    tableSize = tableSizeNew;

    if (staticImgBuffer != NULL) {
        SDL_DestroyTexture(staticImgBuffer);
    }

    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    staticImgBuffer = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        w, h);

    SDL_SetRenderTarget(renderer, staticImgBuffer);

    cellSize = tableSize / 8;

    setDrawColor(renderer, COLOR_BLACK);
    SDL_RenderClear(renderer);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++)
        {
            SDL_Rect rect = { (int)((double)i * cellSize),(int)((double)j * cellSize),  (int)cellSize,  (int)cellSize };
            if (i % 2 == j % 2)
                setDrawColor(renderer, COLOR_WHITE);
            else
                setDrawColor(renderer, COLOR_BLACK);

            SDL_RenderFillRect(renderer, &rect);

            SVG_renderPiece(renderer, board[j][i], (int)((double)i * cellSize), (int)((double)j * cellSize), cellSize);

        }
    }

    // Számozás
    double fontSize = cellSize / 5;

    for (int i = 0; i < 8; i++) {
        render_char(
            renderer, '8' - i, 
            (int)fontSize, 
            (int)(fontSize / 4), 
            (int)(fontSize / 4) + (int)((double)i * cellSize), 
            i % 2 == 0 ? COLOR_BLACK : COLOR_WHITE);

    }

    for (int i = 0; i < 8; i++) {
        render_char(renderer, 'a' + i, 
            (int)fontSize, 
            (int)((double)i * cellSize + cellSize - fontSize),
            (int)(tableSize - fontSize - fontSize / 4), 
            i % 2 != 0 ? COLOR_BLACK : COLOR_WHITE);

    }


    SDL_RenderPresent(renderer);
}

void renderDynamic(SDL_Renderer* renderer) {


    SDL_SetRenderTarget(renderer, NULL);

    // A pufferelt statikus háttér betöltése, hogy ne kelljen minden kis változásnál újrarenderelni
    SDL_RenderCopy(renderer, staticImgBuffer, NULL, NULL);

    setDrawColor(renderer, COLOR_BLACK);
}

void drawThickrect(SDL_Renderer* renderer,int x, int y, int w, int h) {
    const int THICKNESS = 10;

    for (int i = 0; i < THICKNESS; ++i) {
        SDL_Rect r = { x + i, y + i, w - 2 * i, h - 2 * i };
        SDL_RenderDrawRect(renderer, &r);
    }
}

void displayEval(SDL_Renderer* renderer, Move* move) {
    setDrawColor(renderer, COLOR_HIGH);

    while (move != NULL) {

        drawThickrect(renderer, (int)((double)move->pos.file * cellSize), (int)((double)move->pos.rank * cellSize), (int)cellSize, (int)cellSize);

        move = move->next;
    }
}

void highlightCell(SDL_Renderer* renderer,Pos pos) {

    setDrawColor(renderer, COLOR_HIGH);

    drawThickrect(renderer, (int)((double)pos.file* cellSize), (int)((double)pos.rank* cellSize), (int)cellSize, (int)cellSize);
}

void sdlInit(int szeles, int magas, SDL_Window** pwindow, SDL_Renderer** prenderer) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SDL_Log("Nem indithato az SDL: %s", SDL_GetError());
        exit(1);
    }

    SDL_Window* window = SDL_CreateWindow("SDL sakk", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, szeles, magas,SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
    if (window == NULL) {
        SDL_Log("Nem hozhato letre az ablak: %s", SDL_GetError());
        exit(1);
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        SDL_Log("Nem hozhato letre a megjelenito: %s", SDL_GetError());
        exit(1);
    }
    SDL_RenderClear(renderer);

    *pwindow = window;
    *prenderer = renderer;
}