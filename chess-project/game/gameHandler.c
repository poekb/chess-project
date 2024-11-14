

#include "../ai/chessBot.h"
#include "../ai/transpositionTable.h"
#include "../graphics/boardRenderer.h"
#include "../graphics/layout.h"
#include "../graphics/textRenderer.h"
#include "notations.h"
#include "pgn.h"
#include "gameHandler.h"

void recalcUIData();

void UpdateBoard(Board* board, GameData* gameData);
int hasMove(Uint8 start, Uint8 target, GameData* gameData);
int countMoves(Board* board, int depth);

void freeMoveHistory(GameData* gameData);
void freeMoveFuture(GameData* gameData);

bool* nextEnabled;
bool* prevEnabled;
bool* gameLoadEnabled;
Sint32 mouseX;
Sint32 mouseY;

GameData* displayedGameData;

void perftTest(int depth, GameData* gameData);

/*
    The main update loop of the program
    It handles the interractions with the window
*/
void updateLoop() {

    GameData* gameData = malloc(sizeof(GameData));
    if (gameData == NULL) {
        printf("Failed memory allocation!\n");
        exit(1);
    }

    gameData->selectedPos = -1;
    gameData->howerPos = -1;
    gameData->moveCount = 0;
    gameData->bot = false;
    strncpy_s(gameData->startFEN, sizeof(gameData->startFEN), DefaultBoardFEN, sizeof(gameData->startFEN) - 1);
    gameData->startFEN[sizeof(gameData->startFEN) - 1] = '\0';

    gameData->moveHistory = NULL;
    gameData->moveFuture = NULL;

    gameData->isPromotingPiece = false;

    gameData->nextEnabled = false;
    gameData->prevEnabled = false;
    gameData->gameLoadEnabled = true;
    nextEnabled = &gameData->nextEnabled;
    prevEnabled = &gameData->prevEnabled;
    gameLoadEnabled = &gameData->gameLoadEnabled;


    gameData->board = (Board*)malloc(sizeof(Board));
    if (gameData->board == NULL) {
        printf("Failed memory allocation!\n");
        exit(1);
    }


    displayedGameData = gameData;

    recalcUIData();
    loadFEN(gameData->startFEN, gameData);
    initTranspositionTable();

    // Main update loop
    bool quit = false;
    SDL_Event event;
    while (!quit) {
        SDL_WaitEvent(&event);
        switch (event.type) {
        case SDL_KEYDOWN:

            
            renderStatic(renderer);
            UpdateBoard(gameData->board, gameData);

            break;
        case SDL_MOUSEMOTION:
            mouseX = event.motion.x;
            mouseY = event.motion.y;
            gameData->howerPos = getCell(event.motion.x, event.motion.y);

            UpdateBoard(gameData->board, gameData);

            break;
        case SDL_MOUSEBUTTONDOWN: {

            if (gameData->isPromotingPiece) {
                if (gameData->howerPos != -1) {
                    PieceType type = getPromotionPieceTypeByCell(gameData->promotingPieceMove, gameData->howerPos, gameData->board->isWhitesMove);

                    if (type != None) {
                        Uint16 flag = 0;
                        switch (type)
                        {
                        case Queen:
                            flag = PromoteToQueenFlag;
                            break;
                        case Rook:
                            flag = PromoteToRookFlag;
                            break;
                        case Bishop:
                            flag = PromoteToBishopFlag;
                            break;
                        case Knight:
                            flag = PromoteToKnightFlag;
                            break;
                        }

                        gameData->promotingPieceMove &= 0b0000111111111111;
                        gameData->promotingPieceMove |= (flag << 12);

                        stashMove(gameData->promotingPieceMove, gameData);
                        gameData->isPromotingPiece = false;

                        renderStatic(renderer);
                        renderPieces(renderer, gameData->board->square);
                        UpdateBoard(gameData->board, gameData);

                        if (gameData->bot) {
                            UpdateBoard(gameData->board, gameData);

                            Move bestMove = startBot(gameData->board);
                            if (bestMove != 0)
                                stashMove(bestMove, gameData);

                            UpdateBoard(gameData->board, gameData);
                        }
                    }
                }
                gameData->isPromotingPiece = false;
                break;
            }

            testButonClicks();

            int moveIndex = hasMove(gameData->selectedPos, gameData->howerPos, gameData);
            if (gameData->selectedPos != -1 && (moveIndex != -1)) {
                gameData->move = gameData->validMoves[moveIndex];

                if (isPromotion(gameData->move)) {
                    gameData->isPromotingPiece = true;
                    gameData->promotingPieceMove = gameData->move;
                }
                else {
                    stashMove(gameData->move, gameData);
                }

                renderStatic(renderer);
                renderPieces(renderer, gameData->board->square);

                if (gameData->bot && !isPromotion(gameData->move)) {
                    UpdateBoard(gameData->board, gameData);

                    Move bestMove = startBot(gameData->board);
                    if(bestMove != 0)
                        stashMove(bestMove, gameData);
                }

                gameData->selectedPos = -1;
            }
            else {
                gameData->selectedPos = gameData->howerPos;
            }
            UpdateBoard(gameData->board, gameData);

            break;
        }
        case SDL_WINDOWEVENT: {
            recalcUIData();
            UpdateBoard(gameData->board, gameData);

            break;
        }
        case SDL_QUIT:
            quit = true;
            break;
        }
    }

    free(gameData->board);
    while (gameData->moveFuture != NULL) {
        MoveList* current = gameData->moveFuture;
        gameData->moveFuture = gameData->moveFuture->next;
        free(current);
    }
    while (gameData->moveHistory != NULL) {
        MoveList* current = gameData->moveHistory;
        gameData->moveHistory = gameData->moveHistory->next;
        free(current);
    }

    free(transpositionTable);
    free(gameData);
}

void toggleBot() {
    
    displayedGameData->bot = !displayedGameData->bot;
    strcpy(toggleBotButton.text, displayedGameData->bot ? "Stop Bot" : "Start Bot");
    renderButtons();
    SDL_RenderPresent(renderer);
    if (displayedGameData->bot) {
        Move bestMove = startBot(displayedGameData->board);
        if (bestMove != 0)
            stashMove(bestMove, displayedGameData);
    }
}

// Make a move and store it in move history
void stashMove(Move move, GameData* gameData) {

    makeMove(gameData->board,move);

    MoveList* moveList = malloc(sizeof(MoveList));
    if (moveList == NULL) {
        SDL_Log("Unable to allocate memory");
        exit(1);
    }
    moveList->move = move;

    moveList->next = gameData->moveHistory;

    gameData->moveHistory = moveList;

    gameData->nextEnabled = false;
    gameData->prevEnabled = true;

    updateHasGameEnded(gameData);
    
    freeMoveFuture(gameData);
}

void updateHasGameEnded(GameData* gameData) {
    gameData->moveCount = generateMoves(gameData->board, gameData->validMoves, false);

    if (gameData->moveCount == 0) {
        // Game ended
        gameData->board->hasGameEnded = true;
        gameData->board->underAttackMap = generateUnderAttackBitmap(gameData->board);
        if (isCheckPos(gameData->board)) {
            bool whiteWins = !gameData->board->isWhitesMove;
            gameData->board->winnerWhite = whiteWins;
            gameData->board->winnerBlack = !whiteWins;
        }
        else {
            gameData->board->winnerWhite = false;
            gameData->board->winnerBlack = false;
        }
    }
}

// Load move from move future
void nextMove() {

    makeMove(displayedGameData->board, displayedGameData->moveFuture->move);
    MoveList* current = displayedGameData->moveFuture;
    displayedGameData->moveFuture = displayedGameData->moveFuture->next;
    current->next = displayedGameData->moveHistory;
    displayedGameData->moveHistory = current;
    displayedGameData->prevEnabled = true;

    if (displayedGameData->moveFuture == NULL) {
        displayedGameData->nextEnabled = false;
    }

    updateHasGameEnded(displayedGameData);
}

// Load history from move history and store the revoked move in move future
void prevMove() {

    unmakeMove(displayedGameData->board, displayedGameData->moveHistory->move);
    MoveList* current = displayedGameData->moveHistory;
    displayedGameData->moveHistory = displayedGameData->moveHistory->next;
    current->next = displayedGameData->moveFuture;
    displayedGameData->moveFuture = current;
    displayedGameData->nextEnabled = true;

    displayedGameData->board->hasGameEnded = false;

    if (displayedGameData->moveHistory == NULL) {
        displayedGameData->prevEnabled = false;
    }
}

// Go back to the starting position of the game, but store the moves made
void firstMove() {
    while (displayedGameData->prevEnabled) {
        prevMove();
    }
}

// Go to the last future move
void lastMove() {
    while (displayedGameData->nextEnabled) {
        nextMove();
    }
}

void freeMoveHistory(GameData* gameData) {
    while (gameData->moveHistory != NULL) {
        MoveList* current = gameData->moveHistory;
        gameData->moveHistory = gameData->moveHistory->next;
        free(current);
    }
}

void freeMoveFuture(GameData* gameData) {
    while (gameData->moveFuture != NULL) {
        MoveList* current = gameData->moveFuture;
        gameData->moveFuture = gameData->moveFuture->next;
        free(current);
    }
}

/*
    Updates the rendering of the board
    This funcion connects the rendering with the logic
*/
void UpdateBoard(Board* board, GameData* gameData) {

    renderStatic(renderer);

    renderButtons();

    // Display what was the previous move
    if (gameData->prevEnabled) {
        highlightCell(renderer, getStart(gameData->moveHistory->move), (SDL_Color) { 70, 200, 70, 200 });
        highlightCell(renderer, getTarget(gameData->moveHistory->move), (SDL_Color) { 70, 200, 70, 200 });
    }

    // Highlight the selected squre
    if (gameData->selectedPos != -1) {
        highlightCell(renderer, gameData->selectedPos, (SDL_Color) { 100, 100, 200, 150 });
    }

    // Highlight the squre that is howered ower
    if ((gameData->howerPos != -1))
        highlightCell(renderer, gameData->howerPos, (SDL_Color) { 100, 100, 200, 150 });

    // Display the possible moves
    gameData->moveCount = generateMoves(board, gameData->validMoves, false);
    for (int i = 0; i < gameData->moveCount; i++) {
        Uint8 start = getStart(gameData->validMoves[i]);
        Uint8 target = getTarget(gameData->validMoves[i]);

        if (start == gameData->selectedPos) {
            highlightCell(renderer, target, (SDL_Color) { 100, 100, 200, 150 });
        }
    }

    renderPieces(renderer, board->square);
    renderWinner(board);

    if (gameData->isPromotingPiece)
        displayPromotionSelect(renderer, gameData->promotingPieceMove, board->isWhitesMove);

    SDL_RenderPresent(renderer);
}

void resetBoard() {
    loadFEN(DefaultBoardFEN, displayedGameData);
}

void pasteFEN() {
    char* clipBoard = SDL_GetClipboardText();

    loadFEN(clipBoard, displayedGameData);
    SDL_free(clipBoard);
}

void pastePGN() {
    char* clipBoard = SDL_GetClipboardText();

    loadGameFromPGN(displayedGameData->board, clipBoard, displayedGameData);
    SDL_free(clipBoard);
}

void copyFEN() {
    char FEN[100];
    getFENFromBoard(displayedGameData->board, FEN);
    SDL_SetClipboardText(FEN);
}

void copyPGN() {
    char* PGN = getPGN(displayedGameData->board, displayedGameData);
    SDL_SetClipboardText(PGN);
    free(PGN);
}

void savePGN() {
    char* PGN = getPGN(displayedGameData->board, displayedGameData);

    FILE* fptr = fopen("game.pgn", "w");

    fprintf(fptr, "%s\n", PGN);

    fclose(fptr);
    free(PGN);
}

void loadPGN() {
    FILE* fptr = fopen("game.pgn", "r");
    if (fptr == NULL) return;

    fseek(fptr, 0L, SEEK_END);
    size_t size = ftell(fptr);
    rewind(fptr);

    char* PGN = calloc(size + 1, sizeof(char));

    if (PGN == NULL) return;

    fread(PGN, sizeof(char), size, fptr);
    fclose(fptr);

    loadGameFromPGN(displayedGameData->board, PGN, displayedGameData);
    free(PGN);
}

void loadFEN(char* fenStr, GameData* gameData) {
    memcpy(gameData->startFEN, fenStr, min(strlen(fenStr) + 1,100));
    gameData->startFEN[99] = '\0';

    freeMoveFuture(gameData);
    freeMoveHistory(gameData);
    gameData->prevEnabled = false;
    gameData->nextEnabled = false;

    if (gameData->board != NULL) free(gameData->board);

    gameData->board = calloc(1, sizeof(Board));
    if (gameData->board == NULL) {
        SDL_Log("Unable to allocate memory");
        exit(1);
    }

    LoadBoardFromFEN(gameData->board, fenStr);

    // Optional perft test for validating the move generator:
    perftTest(5, gameData);
}

// Test the number of possible moves, for validating the move generator
void perftTest(int depth, GameData* gameData) {
    Uint64 oldTick = SDL_GetTicks64();
    for (int i = 1; i <= depth; i++) {
        printf("Num of possible moves (%d): %10d ", i, countMoves(gameData->board, i));
        printf("in %lu ms\n", (int)(SDL_GetTicks64() - oldTick));
    }
}
int countMoves(Board* board, int depth) {
    if (depth == 0) return 1;

    int sum = 0; // temp 1
    Move* moves = malloc(sizeof(Move) * 100);
    if (moves == NULL) return 1;
    int moveCount = generateMoves(board, moves, false);

    for (int i = 0; i < moveCount; i++) {
        makeMove(board, moves[i]);
        
        int count = countMoves(board, depth - 1);
        if (isPromotion(moves[i]))
            sum += count;
        else
            sum += count;
        unmakeMove(board, moves[i]);
    }
    free(moves);
    return max(sum, 0);
}

// Check if the move is part of the valid moves
int hasMove(Uint8 start, Uint8 target, GameData* gameData) {
    for (int i = 0; i < gameData->moveCount; i++) {
        Uint8 s = getStart(gameData->validMoves[i]);
        Uint8 t = getTarget(gameData->validMoves[i]);

        if (s == start && t == target) {

            return i;
        }
    }
    return -1;
}

// Return the next move in move future, this is important for PGN generation
Move getNextMove(GameData* gameData) {
    return gameData->moveFuture->move;
}
