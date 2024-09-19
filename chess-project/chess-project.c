#include "chess-project.h"

#include "graphics/board-renderer.h"
#include "logic/move-eval.h"

GamePosition gamePosition = {
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
    },
    8,//En Passant-ozható gyalog
    0,
};

SDL_Renderer* renderer;

Pos selectedPos = { 9,9 };
Pos howerPos = { -1,-1 };
PossibleMove* moves = NULL;

void handleClick(double boardSize) {
    if (!(howerPos.rank >= 0 && howerPos.file >= 0 && howerPos.rank < 8 && howerPos.file < 8)) return;

    if (selectedPos.rank != 9) {
        gamePosition.board[howerPos.rank][howerPos.file] = gamePosition.board[selectedPos.rank][selectedPos.file];
        gamePosition.board[selectedPos.rank][selectedPos.file] = 0;
        renderBoard(renderer, boardSize, gamePosition.board);
        renderDynamic(renderer);
        highlightCell(renderer, howerPos);

        SDL_RenderPresent(renderer);
        selectedPos = (Pos){ 9,9 };
        clearMoves(moves);
        moves = NULL;
        return;
    }

    

    // Render possible movess:
    clearMoves(moves);
    moves = getPossibleMoves(gamePosition, howerPos);

    // Ha nem lehet mozgatni a kiválasztott figurát, akkor lépjünk ki a függvényből:
    if (moves == NULL) return;

    selectedPos = howerPos;
    renderDynamic(renderer);
    highlightCell(renderer, selectedPos);

    displayEval(renderer, moves);

    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    enum { ABLAK = 720 };

    sdlInit(ABLAK, ABLAK, &window, &renderer);
    rendererInit(renderer);

    double boardSize = ABLAK;

    renderBoard(renderer, boardSize, gamePosition.board);


    bool quit = false;
    SDL_Event event;
    while (!quit) {
        SDL_WaitEvent(&event);
        int w, h;

        switch (event.type) {
        case SDL_MOUSEMOTION:


            renderDynamic(renderer);

            howerPos = (Pos){ (int)(event.motion.x * 8 / boardSize), (int)(event.motion.y * 8 / boardSize) };

            if (selectedPos.rank != 9) {
                highlightCell(renderer, selectedPos);
            }
            
            if((howerPos.rank >= 0 && howerPos.file >= 0 && howerPos.rank < 8 && howerPos.file < 8))
                highlightCell(renderer, howerPos);

            displayEval(renderer, moves);

            SDL_RenderPresent(renderer);

            break;
        case SDL_MOUSEBUTTONDOWN:
            handleClick(boardSize);
            
            break;
        case SDL_WINDOWEVENT:
            SDL_GetWindowSize(window, &w, &h);
            boardSize = min(w, h);

            renderBoard(renderer, boardSize, gamePosition.board);
            renderDynamic(renderer);

            break;

        case SDL_QUIT:
            quit = true;
            break;
        }
    }
    clearMoves(moves);

    rederer_cleanUp();

    SDL_Quit();

    return 0;
}