#include "svg-loader.h"
#include "text-renderer.h"

#include "board-renderer.h"

const SDL_Color COLOR_BLACK = { 0x5a, 0x40, 0x30, 0xFF };
const SDL_Color COLOR_WHITE = { 0xFF, 0xF0, 0xD0, 0xFF };
const SDL_Color COLOR_HIGH = { 0x70, 0x70, 0xf0, 0xa0 };


SDL_Texture* staticImgBuffer;

SDL_Rect getCellRect(int x, int y);

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

double boardSize = 0;
int offsetX = 0;
int offsetY = 0;
double cellSize;

void renderBoard(SDL_Renderer* renderer, int boardSizeNew, int boardOffsetX, int boardOffsetY) {
    boardSize = boardSizeNew;
    offsetX = boardOffsetX;
    offsetY = boardOffsetY;

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

    cellSize = boardSize / 8;

    setDrawColor(renderer, COLOR_BLACK);
    SDL_RenderClear(renderer);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++)
        {
            SDL_Rect rect = getCellRect(i, j);
            if (i % 2 == j % 2)
                setDrawColor(renderer, COLOR_WHITE);
            else
                setDrawColor(renderer, COLOR_BLACK);

            SDL_RenderFillRect(renderer, &rect);


        }
    }

    // Számozás
    double fontSize = cellSize / 5;

    for (int i = 0; i < 8; i++) {
        render_char(
            renderer, '8' - i, 
            (int)fontSize, 
            offsetX + (int)(fontSize / 4), 
            offsetY + (int)(fontSize / 4) + (int)((double)i * cellSize), 
            i % 2 == 0 ? COLOR_BLACK : COLOR_WHITE);

    }

    for (int i = 0; i < 8; i++) {
        render_char(renderer, 'a' + i, 
            (int)fontSize, 
            offsetX + (int)((double)i * cellSize + cellSize - fontSize),
            offsetY + (int)(boardSize - fontSize - fontSize / 4),
            i % 2 != 0 ? COLOR_BLACK : COLOR_WHITE);

    }

    SDL_RenderPresent(renderer);
}

SDL_Rect getCellRect(int x, int y) {
    return (SDL_Rect){ 
        offsetX + (int)floor(cellSize * x),
        offsetY + (int)floor(cellSize * y),
        (int)(floor(cellSize * (x + 1)) - floor(cellSize * x)),
        (int)(floor(cellSize * (y + 1)) - floor(cellSize * y))
     };
}

void renderPieces(SDL_Renderer* renderer, Piece board[64]) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++)
        {
            SVG_renderPiece(renderer, board[j * 8 + i], getCellRect(i,j));
        }
    }
}

void renderDynamic(SDL_Renderer* renderer) {

    SDL_SetRenderTarget(renderer, NULL);

    // A pufferelt statikus háttér betöltése, hogy ne kelljen minden kis változásnál újrarenderelni
    SDL_RenderCopy(renderer, staticImgBuffer, NULL, NULL);

    setDrawColor(renderer, COLOR_BLACK);
}

static void drawThickrect(SDL_Renderer* renderer, SDL_Rect rect) {
    SDL_RenderFillRect(renderer, &rect);
}

void highlightCell(SDL_Renderer* renderer,Pos pos, SDL_Color color) {

    setDrawColor(renderer, color);

    drawThickrect(renderer, getCellRect(pos.file, pos.rank));
}

void highlightCells(SDL_Renderer* renderer, Uint8* positions, Uint8 count, SDL_Color color) {
    for (int i = 0; i < count; i++) {
        Uint8 pos = positions[i];
        Uint8 rank = pos / 8;
        Uint8 file = pos % 8;
        setDrawColor(renderer, color);
        highlightCell(renderer, (Pos) { file, rank },color);
    }
}

void displayBitboard(SDL_Renderer* renderer, Uint64 bitboard, SDL_Color color) {
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            int index = x + 8 * y;

            if ((bitboard & ((Uint64)1 << index)) != 0)
                highlightCell(renderer, (Pos) { x, y }, color);
        }
    }
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