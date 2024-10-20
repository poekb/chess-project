#include "svg-loader.h"
#include "text-renderer.h"

#include "board-renderer.h"




SDL_Texture* staticImgBuffer;

SDL_Rect getCellRect(int x, int y);
SDL_Rect getCellRectRaw(int x, int y);

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

int boardSize = 0;
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

    staticImgBuffer = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        boardSize, boardSize);

    SDL_SetRenderTarget(renderer, staticImgBuffer);

    cellSize = (double)boardSize / 8;

    setDrawColor(renderer, COLOR_BLACK);
    SDL_RenderClear(renderer);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++)
        {
            SDL_Rect rect = getCellRectRaw(i, j);
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
            (int)(fontSize / 4), 
            (int)(fontSize / 4) + (int)((double)i * cellSize), 
            i % 2 == 0 ? COLOR_BLACK : COLOR_WHITE);

    }

    for (int i = 0; i < 8; i++) {
        render_char(renderer, 'a' + i, 
            (int)fontSize, 
            (int)((double)i * cellSize + cellSize - fontSize),
            (int)(boardSize - fontSize - fontSize / 4),
            i % 2 != 0 ? COLOR_BLACK : COLOR_WHITE);

    }

    SDL_RenderPresent(renderer);
}

Uint8 getCell(int x, int y) {
    int cellX = (int)((x - offsetX) / cellSize);
    int cellY = (int)((y - offsetY) / cellSize);

    if (cellX >= 0 && cellX < 8 && cellY >= 0 && cellY < 8) {
        return cellX + 8 * cellY;
    }
    return -1;
}

SDL_Rect getCellRectRaw(int x, int y) {
    return (SDL_Rect) {
        (int)floor(cellSize * x),
        (int)floor(cellSize * y),
        (int)(floor(cellSize * (x + 1)) - floor(cellSize * x)),
        (int)(floor(cellSize * (y + 1)) - floor(cellSize * y))
    };
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
    setDrawColor(renderer, COLOR_BACKGROUND);
    SDL_RenderClear(renderer);

    // A pufferelt statikus háttér betöltése, hogy ne kelljen minden kis változásnál újrarenderelni
    SDL_RenderCopy(renderer, staticImgBuffer, NULL, &((SDL_Rect) {offsetX,offsetY, boardSize, boardSize}));

    setDrawColor(renderer, COLOR_BLACK);
}

static void drawThickrect(SDL_Renderer* renderer, SDL_Rect rect) {
    SDL_RenderFillRect(renderer, &rect);
}

void highlightCell(SDL_Renderer* renderer,Uint8 pos, SDL_Color color) {

    setDrawColor(renderer, color);

    drawThickrect(renderer, getCellRect(pos % 8, pos / 8));
}

void highlightCells(SDL_Renderer* renderer, Uint8* positions, Uint8 count, SDL_Color color) {
    for (int i = 0; i < count; i++) {
        Uint8 pos = positions[i];
        setDrawColor(renderer, color);
        highlightCell(renderer, pos, color);
    }
}

void displayBitboard(SDL_Renderer* renderer, Uint64 bitboard, SDL_Color color) {
    for (int i = 0; i < 64; i++) {
        if ((bitboard & ((Uint64)1 << i)) != 0)
            highlightCell(renderer, i, color);
    }
}

void sdlInit(int width, int height, SDL_Window** pwindow, SDL_Renderer** prenderer) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SDL_Log("Nem indithato az SDL: %s", SDL_GetError());
        exit(1);
    }

    SDL_Window* window = SDL_CreateWindow("SDL sakk", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
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