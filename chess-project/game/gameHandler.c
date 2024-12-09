#include "../ai/chessBot.h"
#include "../ai/transpositionTable.h"
#include "../graphics/boardRenderer.h"
#include "../graphics/layout.h"
#include "../graphics/textRenderer.h"
#include "notations.h"
#include "pgn.h"
#include "gameHandler.h"

void RecalcUIData();

void UpdateBoard(Board* board, GameData* gameData);

int HasMove(Uint8 start, Uint8 target, GameData* gameData);

int CountMoves(Board* board, int depth);

void FreeMoveHistory(GameData* gameData);

void FreeMoveFuture(GameData* gameData);

bool* nextEnabled;
bool* prevEnabled;
bool* gameLoadEnabled;
Sint32 mouseX;
Sint32 mouseY;

GameData* displayedGameData;

void PerftTest(int depth, GameData* gameData);

GameData* CreateGameData() {
    GameData* gameData = malloc(sizeof(GameData));
    if (!gameData) return NULL;
    gameData->moveFuture = NULL; // Initialize to NULL.
    return gameData;
}

/*
    The main update loop of the program
    It handles the interractions with the window
*/
void UpdateLoop() {
    GameData* gameData = CreateGameData();
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


    gameData->board = (Board*) malloc(sizeof(Board));
    if (gameData->board == NULL) {
        printf("Failed memory allocation!\n");
        exit(1);
    }


    displayedGameData = gameData;

    RecalcUIData();
    LoadFEN(gameData->startFEN, gameData);
    InitTranspositionTable();

    // Main update loop
    bool quit = false;
    SDL_Event event;
    while (!quit) {
        SDL_WaitEvent(&event);
        switch (event.type) {
            case SDL_KEYDOWN:


                RenderStatic(renderer);
                UpdateBoard(gameData->board, gameData);

                break;
            case SDL_MOUSEMOTION:
                mouseX = event.motion.x;
                mouseY = event.motion.y;
                gameData->howerPos = GetCell(event.motion.x, event.motion.y);

                UpdateBoard(gameData->board, gameData);

                break;
            case SDL_MOUSEBUTTONDOWN: {
                if (gameData->isPromotingPiece) {
                    if (gameData->howerPos != -1) {
                        PieceType type = GetPromotionPieceTypeByCell(gameData->promotingPieceMove, gameData->howerPos,
                                                                     gameData->board->isWhitesMove);

                        if (type != None) {
                            Uint16 flag = 0;
                            switch (type) {
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

                            StashMove(gameData->promotingPieceMove, gameData);
                            gameData->isPromotingPiece = false;

                            RenderStatic(renderer);
                            RenderPieces(renderer, gameData->board->square);
                            UpdateBoard(gameData->board, gameData);

                            if (gameData->bot) {
                                UpdateBoard(gameData->board, gameData);

                                Move bestMove = StartBot(gameData->board);
                                if (bestMove != 0)
                                    StashMove(bestMove, gameData);

                                UpdateBoard(gameData->board, gameData);
                            }
                        }
                    }
                    gameData->isPromotingPiece = false;
                    break;
                }

                TestButonClicks();

                int moveIndex = HasMove(gameData->selectedPos, gameData->howerPos, gameData);
                if (gameData->selectedPos != -1 && (moveIndex != -1)) {
                    gameData->move = gameData->validMoves[moveIndex];

                    if (IsPromotion(gameData->move)) {
                        gameData->isPromotingPiece = true;
                        gameData->promotingPieceMove = gameData->move;
                    } else {
                        StashMove(gameData->move, gameData);
                    }

                    RenderStatic(renderer);
                    RenderPieces(renderer, gameData->board->square);

                    if (gameData->bot && !IsPromotion(gameData->move)) {
                        UpdateBoard(gameData->board, gameData);

                        Move bestMove = StartBot(gameData->board);
                        if (bestMove != 0)
                            StashMove(bestMove, gameData);
                    }

                    gameData->selectedPos = -1;
                } else {
                    gameData->selectedPos = gameData->howerPos;
                }
                UpdateBoard(gameData->board, gameData);

                break;
            }
            case SDL_WINDOWEVENT: {
                RecalcUIData();
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

void ToggleBot() {
    displayedGameData->bot = !displayedGameData->bot;
    strcpy(toggleBotButton.text, displayedGameData->bot ? "Stop Bot" : "Start Bot");
    RenderButtons();
    SDL_RenderPresent(renderer);
    if (displayedGameData->bot) {
        Move bestMove = StartBot(displayedGameData->board);
        if (bestMove != 0)
            StashMove(bestMove, displayedGameData);
    }
}

// Make a move and store it in move history
void StashMove(Move move, GameData* gameData) {
    MakeMove(gameData->board, move);

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

    UpdateHasGameEnded(gameData);

    FreeMoveFuture(gameData);
}

void UpdateHasGameEnded(GameData* gameData) {
    gameData->moveCount = GenerateMoves(gameData->board, gameData->validMoves, false);

    if (gameData->moveCount == 0) {
        // Game ended
        gameData->board->hasGameEnded = true;
        gameData->board->underAttackMap = GenerateUnderAttackBitmap(gameData->board);
        if (IsCheckPos(gameData->board)) {
            bool whiteWins = !gameData->board->isWhitesMove;
            gameData->board->winnerWhite = whiteWins;
            gameData->board->winnerBlack = !whiteWins;
        } else {
            gameData->board->winnerWhite = false;
            gameData->board->winnerBlack = false;
        }
    }
}

// Load move from move future
void NextMove() {
    MakeMove(displayedGameData->board, displayedGameData->moveFuture->move);
    MoveList* current = displayedGameData->moveFuture;
    displayedGameData->moveFuture = displayedGameData->moveFuture->next;
    current->next = displayedGameData->moveHistory;
    displayedGameData->moveHistory = current;
    displayedGameData->prevEnabled = true;

    if (displayedGameData->moveFuture == NULL) {
        displayedGameData->nextEnabled = false;
    }

    UpdateHasGameEnded(displayedGameData);
}

// Load history from move history and store the revoked move in move future
void PrevMove() {
    UnmakeMove(displayedGameData->board, displayedGameData->moveHistory->move);
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
void FirstMove() {
    while (displayedGameData->prevEnabled) {
        PrevMove();
    }
}

// Go to the last future move
void LastMove() {
    while (displayedGameData->nextEnabled) {
        NextMove();
    }
}

void FreeMoveHistory(GameData* gameData) {
    if (gameData == NULL) return;
    MoveList* current = gameData->moveHistory;
    while (current != NULL) {
        MoveList* next = current->next;
        free(current);
        current = next;
    }
    gameData->moveHistory = NULL;
}

void FreeMoveFuture(GameData* gameData) {
    if (gameData == NULL) return;
    MoveList* current = gameData->moveFuture;
    while (current != NULL) {
        MoveList* next = current->next;
        free(current);
        current = next;
    }
    gameData->moveFuture = NULL;
}

/*
    Updates the rendering of the board
    This funcion connects the rendering with the logic
*/
void UpdateBoard(Board* board, GameData* gameData) {
    RenderStatic(renderer);

    RenderButtons();

    // Display what was the previous move
    if (gameData->prevEnabled) {
        HighlightCell(renderer, GetStart(gameData->moveHistory->move), (SDL_Color){70, 200, 70, 200});
        HighlightCell(renderer, GetTarget(gameData->moveHistory->move), (SDL_Color){70, 200, 70, 200});
    }

    // Highlight the selected squre
    if (gameData->selectedPos != -1) {
        HighlightCell(renderer, gameData->selectedPos, (SDL_Color){100, 100, 200, 150});
    }

    // Highlight the squre that is howered ower
    if ((gameData->howerPos != -1))
        HighlightCell(renderer, gameData->howerPos, (SDL_Color){100, 100, 200, 150});

    // Display the possible moves
    gameData->moveCount = GenerateMoves(board, gameData->validMoves, false);
    for (int i = 0; i < gameData->moveCount; i++) {
        Uint8 start = GetStart(gameData->validMoves[i]);
        Uint8 target = GetTarget(gameData->validMoves[i]);

        if (start == gameData->selectedPos) {
            HighlightCell(renderer, target, (SDL_Color){100, 100, 200, 150});
        }
    }

    RenderPieces(renderer, board->square);
    RenderWinner(board);

    if (gameData->isPromotingPiece)
        DisplayPromotionSelect(renderer, gameData->promotingPieceMove, board->isWhitesMove);

    SDL_RenderPresent(renderer);
}

void ResetBoard() {
    LoadFEN(DefaultBoardFEN, displayedGameData);
}

void PasteFEN() {
    char* clipBoard = SDL_GetClipboardText();

    LoadFEN(clipBoard, displayedGameData);
    SDL_free(clipBoard);
}

void PastePGN() {
    char* clipBoard = SDL_GetClipboardText();

    LoadGameFromPGN(displayedGameData->board, clipBoard, displayedGameData);
    SDL_free(clipBoard);
}

void CopyFEN() {
    char FEN[100];
    GetFENFromBoard(displayedGameData->board, FEN);
    SDL_SetClipboardText(FEN);
}

void CopyPGN() {
    char* PGN = GetPGN(displayedGameData->board, displayedGameData);
    SDL_SetClipboardText(PGN);
    free(PGN);
}

void SavePGN() {
    char* PGN = GetPGN(displayedGameData->board, displayedGameData);

    FILE* fptr = fopen("game.pgn", "w");

    fprintf(fptr, "%s\n", PGN);

    fclose(fptr);
    free(PGN);
}

void LoadPGN() {
    FILE* fptr = fopen("game.pgn", "r");
    if (fptr == NULL) return;

    fseek(fptr, 0L, SEEK_END);
    size_t size = ftell(fptr);
    rewind(fptr);

    char* PGN = calloc(size + 1, sizeof(char));

    if (PGN == NULL) return;

    fread(PGN, sizeof(char), size, fptr);
    fclose(fptr);

    LoadGameFromPGN(displayedGameData->board, PGN, displayedGameData);
    free(PGN);
}

void LoadFEN(char* fenStr, GameData* gameData) {
    FreeMoveFuture(gameData);
    FreeMoveHistory(gameData);

    memcpy(gameData->startFEN, fenStr, min(strlen(fenStr) + 1, 100));
    gameData->startFEN[99] = '\0';

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
    //PerftTest(5, gameData);
}

// Test the number of possible moves, for validating the move generator
void PerftTest(int depth, GameData* gameData) {
    Uint64 oldTick = SDL_GetTicks64();
    for (int i = 1; i <= depth; i++) {
        printf("Perft (%d): %10d ", i, CountMoves(gameData->board, i));
        printf("in %lu ms\n", (int) (SDL_GetTicks64() - oldTick));
    }
}

int CountMoves(Board* board, int depth) {
    if (depth == 0) return 1;

    int sum = 0; // temp 1
    Move* moves = malloc(sizeof(Move) * 100);
    if (moves == NULL) return 1;
    int moveCount = GenerateMoves(board, moves, false);

    for (int i = 0; i < moveCount; i++) {
        MakeMove(board, moves[i]);

        int count = CountMoves(board, depth - 1);
        if (IsPromotion(moves[i]))
            sum += count;
        else
            sum += count;
        UnmakeMove(board, moves[i]);
    }
    free(moves);
    return max(sum, 0);
}

// Check if the move is part of the valid moves
int HasMove(Uint8 start, Uint8 target, GameData* gameData) {
    for (int i = 0; i < gameData->moveCount; i++) {
        Uint8 s = GetStart(gameData->validMoves[i]);
        Uint8 t = GetTarget(gameData->validMoves[i]);

        if (s == start && t == target) {
            return i;
        }
    }
    return -1;
}

// Return the next move in move future, this is important for PGN generation
Move GetNextMove(GameData* gameData) {
    return gameData->moveFuture->move;
}
