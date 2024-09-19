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

    Pos selected = { 9,9 };
    Pos highlight = { -1,-1 };


    bool quit = false;
    SDL_Event event;
    while (!quit) {
        SDL_WaitEvent(&event);
        int w, h;

        switch (event.type) {
        case SDL_MOUSEMOTION:


            render_dynamic(renderer);

            highlight = (Pos){ event.motion.x * 8 / board_size, event.motion.y * 8 / board_size };

            if (selected.row != 9) {
                high_light_cell(renderer, selected.col, selected.row);
            }
            
            high_light_cell(renderer, highlight.col, highlight.row);


            SDL_RenderPresent(renderer);

            break;
        case SDL_MOUSEBUTTONDOWN:

            if (highlight.row >= 0 && highlight.col >= 0 && highlight.row < 8 && highlight.col < 8) {
                if (selected.row != 9) {
                    position.board[highlight.col][highlight.row] = position.board[selected.col][selected.row];
                    position.board[selected.col][selected.row] = 0;
                    render_board(renderer, board_size, position.board);
                    render_dynamic(renderer);
                    SDL_RenderPresent(renderer);
                    selected = (Pos){ 9,9 };
                    break;
                }

                selected = highlight;
                render_dynamic(renderer);
                high_light_cell(renderer, selected.col, selected.row);
                SDL_RenderPresent(renderer);

            }
            break;
        case SDL_WINDOWEVENT:
            SDL_GetWindowSize(window, &w, &h);
            board_size = min(w, h);

            render_board(renderer, board_size, position.board);
            render_dynamic(renderer);

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