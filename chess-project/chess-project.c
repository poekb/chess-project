#include "chess-project.h"
#include "board-renderer.h"

#include <math.h>
#include <SDL_ttf.h>

Position position = {
    WHITE,
    {
        {ROOK_D,KNIGHT_D,BISHOP_D,QUEEN_D,KING_D,BISHOP_D,KNIGHT_D,ROOK_D},
        {PAWN_D,PAWN_D,PAWN_D,PAWN_D,PAWN_D,PAWN_D,PAWN_D,PAWN_D},
        {0},
        {0},
        {0},
        {0},
        {PAWN_L,PAWN_L,PAWN_L,PAWN_L,PAWN_L,PAWN_L,PAWN_L,PAWN_L},
        {ROOK_L,KNIGHT_L,BISHOP_L,QUEEN_L,KING_L,BISHOP_L,KNIGHT_L,ROOK_L}
    }
};

SDL_Renderer* renderer;

int main(int argc, char* argv[]) {
    enum { ABLAK = 720 };

    sdl_init(ABLAK, ABLAK, &window, &renderer);
    renderer_init(renderer);

    double board_size = ABLAK;

    render_board(renderer, board_size, position.board);

    bool quit = false;
    SDL_Event event;
    while (!quit) {
        SDL_WaitEvent(&event);
        int w, h;

        switch (event.type) {
        case SDL_MOUSEMOTION:
            render_dynamic(renderer, event);
            break;

        case SDL_WINDOWEVENT:
            SDL_GetWindowSize(window, &w, &h);
            board_size = min(w, h);

            render_board(renderer, board_size, position.board);
            render_dynamic(renderer, event);

            break;

        case SDL_QUIT:
            quit = true;
            break;
        }
    }

    rederer_cleanUp();

    SDL_Quit();

    return 0;
}