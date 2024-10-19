#include "chess-project.h"

#include "graphics/board-renderer.h"
#include "core/move.h"

#include "./core/board.h"
#include "moveGeneration/precompution.h"
#include "moveGeneration/moveGenerator.h"
#include "ai/chess-bot.h"
#include "logic/evaluator.h"

SDL_Renderer* renderer;

Pos selectedPos = { 9,9 };
Pos howerPos = { 9,9 };

Move validMoves[100];
Move move;
int moveCount = 0;


void Update(Board* board);

int hasMove(Pos start, Pos target) {
    for (int i = 0; i < moveCount; i++) {
        Uint8 s = getStart(validMoves[i]);
        Uint8 t = getTarget(validMoves[i]);

        if (s == start.file + start.rank * 8 && t == target.file + target.rank * 8) {

            return i;
        }
    }
    return -1;
}


int countMoves(Board* board, int depth) {
    if (depth == 0) return 1;

    int sum = 0; // temp 1
    Move* moves = malloc(sizeof(Move) * 100);
    if (moves == NULL) return 1;
    int moveCount = generateMoves(board, moves, false);

    for (int i = 0; i < moveCount; i++) {
        MakeMove(board, moves[i]);
        sum += countMoves(board, depth - 1);
        RevokeMove(board, moves[i]);
    }
    free(moves);
    return max(sum,0);
}

bool bot = false;

int main(int argc, char* argv[]) {
    enum { ABLAK = 720 };

    sdlInit(ABLAK, ABLAK, &window, &renderer);
    rendererInit(renderer);

    double boardSize = ABLAK;

    //return 0;


    Board* board = calloc(1,sizeof(Board));
    if (board == NULL) {
        SDL_Log("Unable to allocate memory");
        exit(1);
    }
    preCompute();
    LoadBoardFromFEN(board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    //LoadBoardFromFEN(board, "7k/1p4p1/p4b1p/3N3P/2p5/2rb4/PP2r3/K2R2R1 b - - 0 1");
    
    //LoadBoardFromFEN(board, "3k4/2n2B2/1KP5/2B2p2/5b1p/7P/8/8 b - - 0 0");
    
    //LoadBoardFromFEN(board, "5k2/1B1r3p/3N4/p3P3/2b2P2/P3P3/n2K4/2R5 b - - 0 1");
    //LoadBoardFromFEN(board, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");


    
    if (true) {
        printf("Num of possiblemoves (%d): %10d\n", 2, countMoves(board, 2));
        printf("Num of possiblemoves (%d): %10d\n", 3, countMoves(board, 3));
        printf("Num of possiblemoves (%d): %10d\n", 4, countMoves(board, 4));
        printf("Num of possiblemoves (%d): %10d\n", 5, countMoves(board, 5));
        //printf("Num of possiblemoves (%d): %10d\n", 6, countMoves(board, 6));
    }

    //mesureBot(board, 1);

    printf("Eval: %d\n", evalBoard(board));

    renderBoard(renderer, boardSize - 20, 10,10);

    highlightCells(renderer, board->Knights[0].list, board->Knights[0].count, (SDL_Color) {255,255,255,255});

    renderPieces(renderer, board->square);

    SDL_RenderPresent(renderer);


    bool quit = false;
    SDL_Event event;
    while (!quit) {
        SDL_WaitEvent(&event);
        int w, h;


        
        switch (event.type) {
        case SDL_KEYDOWN:

            bot = !bot;
            CalcBestMove(board);
            
            renderDynamic(renderer);

            Update(board);

            break;
        case SDL_MOUSEMOTION:
            howerPos = (Pos){ (int)(event.motion.x * 8 / boardSize), (int)(event.motion.y * 8 / boardSize) };

            Update(board);
            
            break;
        case SDL_MOUSEBUTTONDOWN: {

            int moveIndex = hasMove(selectedPos, howerPos);
            if (selectedPos.rank != 9 && (moveIndex != -1)) {
                move = validMoves[moveIndex];
                renderDynamic(renderer);

                MakeMove(board, move);
                renderPieces(renderer, board->square);

                Update(board);

                if(bot)
                    CalcBestMove(board);

                Update(board);

                selectedPos.rank = 9;
            }
            else {
                selectedPos = howerPos;
                Update(board);
            }


            break;
        }
        case SDL_WINDOWEVENT:
            
            SDL_GetWindowSize(window, &w, &h);
            boardSize = min(w, h);

            renderBoard(renderer, boardSize - 20, 10, 10);
            Update(board);
            
            break;

        case SDL_QUIT:
            quit = true;
            break;
        }

    }
    
    // Cleanups
    rederer_cleanUp();
    free(board);

    SDL_Quit();

    return 0;
}

void Update(Board* board) {

    renderDynamic(renderer);

    if (selectedPos.rank != -1) {
        highlightCell(renderer, selectedPos, (SDL_Color) { 100, 100, 200, 150 });
    }

    if ((howerPos.rank >= 0 && howerPos.file >= 0 && howerPos.rank < 8 && howerPos.file < 8))
        highlightCell(renderer, howerPos, (SDL_Color) { 100, 100, 200, 150 });

    /*
    highlightCells(renderer, board->Knights[0].list, board->Knights[0].count, (SDL_Color) { 0, 100, 0, 200 });
    highlightCells(renderer, board->Pawns[0].list, board->Pawns[0].count, (SDL_Color) { 50, 50, 50, 200 });
    highlightCells(renderer, board->Bishops[0].list, board->Bishops[0].count, (SDL_Color) { 100, 0, 0, 200 });
    highlightCells(renderer, board->Rooks[0].list, board->Rooks[0].count, (SDL_Color) { 0, 0, 100, 200 });
    highlightCells(renderer, board->Queens[0].list, board->Queens[0].count, (SDL_Color) { 100, 0, 100, 200 });
    */

    moveCount = generateMoves(board, &validMoves, false);

    for (int i = 0; i < moveCount;i++) {
        Uint8 start = getStart(validMoves[i]);
        Uint8 target = getTarget(validMoves[i]);

        if (start == selectedPos.file + selectedPos.rank * 8) {
            highlightCell(renderer, (Pos) {target%8,target/8}, (SDL_Color) { 100, 100, 200, 150 });
        }
    }
    displayBitboard(renderer, board->underAttackMap, (SDL_Color) { 200, 100, 100, 150 });
    highlightCells(renderer, &board->kingSquare[board->isWhitesMove ? WhiteIndex : BlackIndex], 1, (SDL_Color) { 100, 200, 100, 150 });
    renderPieces(renderer, board->square);
    SDL_RenderPresent(renderer);
}