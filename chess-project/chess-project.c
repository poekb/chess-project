#include "chess-project.h"

#include "graphics/board-renderer.h"
#include "graphics/text-renderer.h"

#include "core/move.h"
#include "core/board.h"

#include "moveGeneration/precompution.h"
#include "moveGeneration/moveGenerator.h"

#include "ai/chess-bot.h"

#include "logic/evaluator.h"

#include "game/gameHandler.h"





int main(int argc, char* argv[]) {

    sdlInit(1000, 720, &window, &renderer);
    SDL_SetWindowMinimumSize(window, 480, 480);
    rendererInit(renderer);

    double boardSize = 720;

    preCompute();
    
    //LoadBoardFromFEN(board, "7k/1p4p1/p4b1p/3N3P/2p5/2rb4/PP2r3/K2R2R1 b - - 0 1");
    //LoadBoardFromFEN(board, "3k4/2n2B2/1KP5/2B2p2/5b1p/7P/8/8 b - - 0 0");
    //LoadBoardFromFEN(board, "5k2/1B1r3p/3N4/p3P3/2b2P2/P3P3/n2K4/2R5 b - - 0 1");
    //LoadBoardFromFEN(board, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");

    //if (true) {
    //    printf("Num of possiblemoves (%d): %10d\n", 2, countMoves(board, 2));
    //    printf("Num of possiblemoves (%d): %10d\n", 3, countMoves(board, 3));
    //    printf("Num of possiblemoves (%d): %10d\n", 4, countMoves(board, 4));
    //    printf("Num of possiblemoves (%d): %10d\n", 5, countMoves(board, 5));
    //    //printf("Num of possiblemoves (%d): %10d\n", 6, countMoves(board, 6));
    //}

    //mesureBot(board, 1);

    //printf("Eval: %d\n", evalBoard(board));

    //renderBoard(renderer, boardSize - 20, 10,10);
    //renderPieces(renderer, board->square);
    //renderTextbox(renderer, "Hello World!", (SDL_Rect) { 730, 10, 400, 100 }, (SDL_Color) {100,100,100,255}, (SDL_Color) { 210, 200, 200, 255 });
    
    /*SDL_RenderPresent*/(renderer);

    updateLoop();
    
    // Cleanups
    rederer_cleanUp();
    //free(board);

    SDL_Quit();

    return 0;
}
