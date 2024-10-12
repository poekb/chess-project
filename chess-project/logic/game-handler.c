#include "game-handler.h"

#include "rules.h"
#include "../graphics/board-renderer.h"
#include "evaluator.h"
#include "../ai/chess-bot.h"

Game* game;

Game* getGame() {
    return game;
}

Possibilities* possibilityMatrix[8][8] = { {NULL} };

void initGame() {
    // set up default board
    game = malloc(sizeof(Game));

    if (game == 0) {
        SDL_Log("Unable to allocate memory");
        exit(1);
    }

    game->position = (GamePosition){
        {
            { ROOK_D,KNIGHT_D,BISHOP_D,QUEEN_D,KING_D,BISHOP_D,KNIGHT_D,ROOK_D },
            {PAWN_D,PAWN_D,PAWN_D,PAWN_D,PAWN_D,PAWN_D,PAWN_D,PAWN_D},
            {0},
            {0},
            {0},
            {0},
            {PAWN_L,PAWN_L,PAWN_L,PAWN_L,PAWN_L,PAWN_L,PAWN_L,PAWN_L},
            {ROOK_L,KNIGHT_L,BISHOP_L,QUEEN_L,KING_L,BISHOP_L,KNIGHT_L,ROOK_L}
        },
        WHITE,
        WHITE_KINGSIDE | WHITE_QUEENSIDE | BLACK_KINGSIDE | BLACK_QUEENSIDE,
        -1,//En Passant-ozható gyalog
        0,
        1
    };
    game->next = NULL;
    game->prev = NULL;

    fillPossibilityMatrix();
}
int n = 0;

bool movePieceFromTo(Pos from, Pos to) {
    printf("Mem: %d\n", n);

    Possibilities* possibilities = containsPossibility(possibilityMatrix[from.rank][from.file], to);
    if (possibilities == NULL)
        return false;
    

    Game* newGame = malloc(sizeof(Game));
    if (newGame == NULL) {
        SDL_Log("Unable to allocate memory");
        exit(1);
    }
    memcpy(&(newGame->position), &(possibilities->possibility.rersultPosition), sizeof(GamePosition));

    game->next = newGame;
    newGame->prev = game;
    newGame->next = NULL;

    game = newGame;

    fillPossibilityMatrix();
    printf("%d\n", evalBoard(game->position.board));

    

    return true;
}

void fillPossibilityMatrix() {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            freePossibilities(possibilityMatrix[rank][file]);
            possibilityMatrix[rank][file] = NULL;
            generatePossibleMoves(game->position, (Pos) { file, rank }, &possibilityMatrix[rank][file]);
        }
    }
}


void generatePossibleMoves(GamePosition position, Pos from, Possibilities** end) {
    Move* movesDef = getPossibleMoves(&(position), from);
    Move* moves = movesDef;
    while (moves != NULL) {
        Possibilities* newPossibility = malloc(sizeof(Possibilities));
        if (newPossibility == NULL) {
            SDL_Log("Unable to allocate memory");
            exit(1);
        }


        memcpy(&(newPossibility->possibility.rersultPosition), &(position), sizeof(GamePosition));


        // Move piece
        if (moves->pos.rank == 0 && position.board[from.rank][from.file] == PAWN_L)
            newPossibility->possibility.rersultPosition.board[moves->pos.rank][moves->pos.file] = QUEEN_L;
        else if (moves->pos.rank == 7 && position.board[from.rank][from.file] == PAWN_D)
            newPossibility->possibility.rersultPosition.board[moves->pos.rank][moves->pos.file] = QUEEN_D;
        else
            newPossibility->possibility.rersultPosition.board[moves->pos.rank][moves->pos.file] = position.board[from.rank][from.file];

        newPossibility->possibility.rersultPosition.board[from.rank][from.file] = 0;

        // Castle rules

        if (moves->castle == WHITE_KINGSIDE) {
            newPossibility->possibility.rersultPosition.board[from.rank][from.file + 1] = (position.turn ? ROOK_L : ROOK_D);
            newPossibility->possibility.rersultPosition.board[from.rank][7] = 0;
        }
        if (moves->castle == WHITE_QUEENSIDE) {
            newPossibility->possibility.rersultPosition.board[from.rank][from.file - 1] = (position.turn ? ROOK_L : ROOK_D);
            newPossibility->possibility.rersultPosition.board[from.rank][0] = 0;
        }

        // Disable castle conditions

        if (from.rank == 7 && from.file == 0)
            newPossibility->possibility.rersultPosition.castleConditions &= ~(Uint8)WHITE_QUEENSIDE;
        if (from.rank == 7 && from.file == 7)
            newPossibility->possibility.rersultPosition.castleConditions &= ~(Uint8)WHITE_KINGSIDE;
        if (from.rank == 0 && from.file == 0)
            newPossibility->possibility.rersultPosition.castleConditions &= ~(Uint8)BLACK_QUEENSIDE;
        if (from.rank == 0 && from.file == 7)
            newPossibility->possibility.rersultPosition.castleConditions &= ~(Uint8)BLACK_KINGSIDE;

        if (position.board[from.rank][from.file] == KING_L)
            newPossibility->possibility.rersultPosition.castleConditions &= ~(Uint8)(WHITE_QUEENSIDE | WHITE_KINGSIDE);
        if (position.board[from.rank][from.file] == KING_D)
            newPossibility->possibility.rersultPosition.castleConditions &= ~(Uint8)(BLACK_QUEENSIDE | BLACK_KINGSIDE);

        // En passant
        newPossibility->possibility.rersultPosition.enPassant = moves->enPassant;
        if (moves->takesEnPassant) {
            newPossibility->possibility.rersultPosition.board[from.rank][moves->pos.file] = 0;
        }

        if (isCheck(&(newPossibility->possibility.rersultPosition))) {
            free(newPossibility);
        }
        else {
            newPossibility->possibility.source = from;
            newPossibility->possibility.target = moves->pos;

            newPossibility->possibility.rersultPosition.turn = !newPossibility->possibility.rersultPosition.turn;

            newPossibility->next = *end;
            *end = newPossibility;

            n++;
        }

        moves = moves->next;
    }

    freeMoves(movesDef);
}

void cleanUpGameHandler() {
    while (game->next != NULL) {
        game = game->next;
    }

    while (game->prev != NULL) {
        Game* current = game;
        game = game->prev;

        free(current);
    }
}

Possibilities* containsPossibility(Possibilities* possibilities, Pos move) {

    while (possibilities != NULL) {
        if (possibilities->possibility.target.file == move.file && possibilities->possibility.target.rank == move.rank)
            return possibilities;
        possibilities = possibilities->next;
    }

    return NULL;
}

void freePossibilities(Possibilities* possibilities) {
    if (possibilities != NULL) {
        freePossibilities(possibilities->next);
        free(possibilities);
        n--;
    }
}

