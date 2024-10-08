#include "chess-project.h"

#include "graphics/board-renderer.h"
#include "logic/move-eval.h"
#include "logic/game-handler.h"

SDL_Renderer* renderer;

Pos selectedPos = { 9,9 };
Pos howerPos = { -1,-1 };
Move* moves = NULL;

void handleClick(double boardSize) {
    if (!(howerPos.rank >= 0 && howerPos.file >= 0 && howerPos.rank < 8 && howerPos.file < 8)) return;

    if (selectedPos.rank != 9) {

        
        if (containsMove(moves, howerPos) != NULL) {
            movePieceFromTo(selectedPos, howerPos);
        }

        renderBoard(renderer, boardSize, getGame()->position.board);
        renderDynamic(renderer);
        highlightCell(renderer, howerPos);

        SDL_RenderPresent(renderer);
        selectedPos = (Pos){ 9,9 };
        freeMoves(moves);
        moves = NULL;
        return;
    }

    // Render possible movess:
    freeMoves(moves);
    moves = getPossibleMoves(&getGame()->position, howerPos);

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

    initGame();

    renderBoard(renderer, boardSize, getGame()->position.board);


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

            renderBoard(renderer, boardSize, getGame()->position.board);
            renderDynamic(renderer);

            break;

        case SDL_QUIT:
            quit = true;
            break;
        }
    }

    // Cleanups
    freeMoves(moves);
    rederer_cleanUp();
    cleanUpGameHandler();

    SDL_Quit();

    return 0;
}