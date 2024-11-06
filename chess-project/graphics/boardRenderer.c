#include "svgLoader.h"
#include "textRenderer.h"
#include "layout.h"
#include "boardRenderer.h"

SDL_Texture* staticImgBuffer; // the background for the board

SDL_Rect getCellRect(int x, int y);
SDL_Rect getCellRectRaw(int x, int y);

void setDrawColor(SDL_Renderer* renderer, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void rendererInit(SDL_Renderer* renderer) {

    textRendererInit();
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SVG_init(renderer);
}

void redererCleanup() {
    textRendererCleanup();
    SVG_clear();
}

int boardSize = 0;
int offsetX = 0;
int offsetY = 0;
double cellSize;

// Render the static board background
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

    setDrawColor(renderer, HexToRGBA(COLOR_BLACK));
    SDL_RenderClear(renderer);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++)
        {
            SDL_Rect rect = getCellRectRaw(i, j);
            if (i % 2 == j % 2)
                setDrawColor(renderer, HexToRGBA(COLOR_WHITE));
            else
                setDrawColor(renderer, HexToRGBA(COLOR_BLACK));

            SDL_RenderFillRect(renderer, &rect);


        }
    }

    // Numbering
    double fontSize = cellSize / 5;

    for (int i = 0; i < 8; i++) {
        renderChar(
            renderer, '8' - i, 
            (int)fontSize, 
            (int)(fontSize / 4), 
            (int)(fontSize / 4) + (int)((double)i * cellSize), 
            i % 2 == 0 ? HexToRGBA(COLOR_BLACK) : HexToRGBA(COLOR_WHITE));
    }

    for (int i = 0; i < 8; i++) {
        renderChar(renderer, 'a' + i, 
            (int)fontSize, 
            (int)((double)i * cellSize + cellSize - fontSize),
            (int)(boardSize - fontSize - fontSize / 4),
            i % 2 != 0 ? HexToRGBA(COLOR_BLACK) : HexToRGBA(COLOR_WHITE));
    }

    SDL_RenderPresent(renderer);
}



// Get the current cell, that is howered over
Uint8 getCell(int x, int y) {
    int cellX = (int)((x - offsetX) / cellSize);
    int cellY = (int)((y - offsetY) / cellSize);
    if (x >= offsetX && cellX < 8 && y >= offsetY && cellY < 8) {
        return cellX + 8 * cellY;
    }
    return -1;
}

// Get the rectangle of the cell, but only relative to the upper left corner of the board
SDL_Rect getCellRectRaw(int x, int y) {
    return (SDL_Rect) {
        (int)floor(cellSize * x),
        (int)floor(cellSize * y),
        (int)(floor(cellSize * (x + 1)) - floor(cellSize * x)),
        (int)(floor(cellSize * (y + 1)) - floor(cellSize * y))
    };
}

// Get the rectangle of the cell, relative to the upper left corner of the window
SDL_Rect getCellRect(int x, int y) {
    return (SDL_Rect){ 
        offsetX + (int)floor(cellSize * x),
        offsetY + (int)floor(cellSize * y),
        (int)(floor(cellSize * (x + 1)) - floor(cellSize * x)),
        (int)(floor(cellSize * (y + 1)) - floor(cellSize * y))
     };
}

// Render all pieces that are present on the board
void renderPieces(SDL_Renderer* renderer, Piece board[64]) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++)
        {
            SVG_renderPiece(renderer, board[j * 8 + i], getCellRect(i,j));
        }
    }
}

// Loading in the static board background
void renderStatic(SDL_Renderer* renderer) {
    SDL_SetRenderTarget(renderer, NULL);
    setDrawColor(renderer, HexToRGBA(COLOR_BACKGROUND));
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, staticImgBuffer, NULL, &((SDL_Rect) {offsetX,offsetY, boardSize, boardSize}));

    setDrawColor(renderer, HexToRGBA(COLOR_BLACK));
}

// Highlight a square with set color
void highlightCell(SDL_Renderer* renderer,Uint8 pos, SDL_Color color) {

    setDrawColor(renderer, color);
    SDL_Rect rect = getCellRect(pos % 8, pos / 8);
    SDL_RenderFillRect(renderer, &rect);
}

// Highlight multiple cells with set color
void highlightCells(SDL_Renderer* renderer, Uint8* positions, Uint8 count, SDL_Color color) {
    for (int i = 0; i < count; i++) {
        Uint8 pos = positions[i];
        setDrawColor(renderer, color);
        highlightCell(renderer, pos, color);
    }
}

// Highlight cells, that are enabled in a bitboard
void displayBitboard(SDL_Renderer* renderer, Uint64 bitboard, SDL_Color color) {
    for (int i = 0; i < 64; i++) {
        if ((bitboard & ((Uint64)1 << i)) != 0)
            highlightCell(renderer, i, color);
    }
}

// Initialize SDL
void sdlInit(int width, int height, SDL_Window** pwindow, SDL_Renderer** prenderer) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SDL_Log("SDL could not start: %s", SDL_GetError());
        exit(1);
    }

    SDL_Window* window = SDL_CreateWindow("SDL chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
    if (window == NULL) {
        SDL_Log("Unable to open window: %s", SDL_GetError());
        exit(1);
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        exit(1);
    }
    SDL_RenderClear(renderer);

    *pwindow = window;
    *prenderer = renderer;
}