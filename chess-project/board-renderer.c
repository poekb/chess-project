#include "chess-project.h"
#include "board-renderer.h"
#include "svg-loader.h"
#include "text-renderer.h"

#include <SDL_ttf.h>
#include <SDL_image.h>

const SDL_Color COLOR_BLACK = { 0x5a, 0x40, 0x30, 0xFF };
const SDL_Color COLOR_WHITE = { 0xFF, 0xF0, 0xD0, 0xFF };

SDL_Texture* static_img_buffer;

void set_drawColor(SDL_Renderer* renderer, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void renderer_init(SDL_Renderer* renderer) {

    textRend_init();

    SVGs_init(renderer);
}

void rederer_cleanUp() {
    textRend_clean();

    SVGs_clear();
}

void render_board(SDL_Renderer* renderer, double table_size, Uint8 board[8][8]) {

    if (static_img_buffer != NULL) {
        SDL_DestroyTexture(static_img_buffer);
    }

    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    static_img_buffer = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        w, h);

    SDL_SetRenderTarget(renderer, static_img_buffer);

    double cell_size = table_size / 8;

    set_drawColor(renderer, COLOR_BLACK);
    SDL_RenderClear(renderer);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++)
        {
            SDL_Rect rect = { (int)((double)i * cell_size),(int)((double)j * cell_size),  (int)cell_size,  (int)cell_size };
            if (i % 2 == j % 2)
                set_drawColor(renderer, COLOR_WHITE);
            else
                set_drawColor(renderer, COLOR_BLACK);

            SDL_RenderFillRect(renderer, &rect);

            SVG_renderPiece(renderer, board[j][i], (int)((double)i * cell_size), (int)((double)j * cell_size), cell_size);

        }
    }

    // Számozás
    double font_size = cell_size / 5;

    for (int i = 0; i < 8; i++) {
        render_char(
            renderer, '8' - i, 
            (int)font_size, 
            (int)(font_size / 4), 
            (int)(font_size / 4) + (int)((double)i * cell_size), 
            i % 2 == 0 ? COLOR_BLACK : COLOR_WHITE);

    }

    for (int i = 0; i < 8; i++) {
        render_char(renderer, 'a' + i, 
            (int)font_size, 
            (int)((double)i * cell_size + cell_size - font_size),
            (int)(table_size - font_size - font_size / 4), 
            i % 2 != 0 ? COLOR_BLACK : COLOR_WHITE);

    }


    SDL_RenderPresent(renderer);
}

void render_dynamic(SDL_Renderer* renderer, SDL_Event event) {

    int x = event.motion.x;
    int y = event.motion.y;

    SDL_SetRenderTarget(renderer, NULL);

    // A pufferelt statikus háttér betöltése, hogy ne kelljen minden kis változásnál újrarenderelni
    SDL_RenderCopy(renderer, static_img_buffer, NULL, NULL);

    set_drawColor(renderer, COLOR_BLACK);

    SVG_renderPiece(renderer, 1, x-40, y-40, 80);


    SDL_RenderPresent(renderer);
}

void sdl_init(int szeles, int magas, SDL_Window** pwindow, SDL_Renderer** prenderer) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SDL_Log("Nem indithato az SDL: %s", SDL_GetError());
        exit(1);
    }

    SDL_Window* window = SDL_CreateWindow("SDL sakk", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, szeles, magas,/*SDL_WINDOW_RESIZABLE |*/ SDL_WINDOW_SHOWN);
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