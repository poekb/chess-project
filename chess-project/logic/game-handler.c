#include "game-handler.h"

#include "rules.h"
#include "../graphics/board-renderer.h"
#include "evaluator.h"

Game* game;

Game* getGame() {
    return game;
}

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
}

bool movePieceFromTo(Pos from, Pos to) {
    // TODO: check for validity and special edge cases
    
    Move* possibleMoves = getPossibleMoves(&(game->position), from);

    Move* move = containsMove(possibleMoves, to);
    if(move == NULL) return false;

    Game* nextMove = malloc(sizeof(Game));
    
    if (nextMove == 0) {
        SDL_Log("Unable to allocate memory");
        exit(1);
    }

    memcpy(&(nextMove->position), &(game->position), sizeof(GamePosition));


    // Move piece
    if (to.rank == 0 && game->position.board[from.rank][from.file] == PAWN_L)
        nextMove->position.board[to.rank][to.file] = QUEEN_L;
    else if(to.rank == 7 && game->position.board[from.rank][from.file] == PAWN_D)
        nextMove->position.board[to.rank][to.file] = QUEEN_D;
    else
        nextMove->position.board[to.rank][to.file] = game->position.board[from.rank][from.file];

    nextMove->position.board[from.rank][from.file] = 0;

    // Castle rules

    if (move->castle == WHITE_KINGSIDE) {
        nextMove->position.board[from.rank][from.file + 1] = (game->position.turn ? ROOK_L : ROOK_D);
        nextMove->position.board[from.rank][7] = 0;
    }
    if (move->castle == WHITE_QUEENSIDE) {
        nextMove->position.board[from.rank][from.file - 1] = (game->position.turn ? ROOK_L : ROOK_D);
        nextMove->position.board[from.rank][0] = 0;
    }

    // Disable castle conditions

    if (from.rank == 7 && from.file == 0)
        nextMove->position.castleConditions &= ~(Uint8)WHITE_QUEENSIDE;
    if (from.rank == 7 && from.file == 7)
        nextMove->position.castleConditions &= ~(Uint8)WHITE_KINGSIDE;
    if (from.rank == 0 && from.file == 0)
        nextMove->position.castleConditions &= ~(Uint8)BLACK_QUEENSIDE;
    if (from.rank == 0 && from.file == 7)
        nextMove->position.castleConditions &= ~(Uint8)BLACK_KINGSIDE;

    if(game->position.board[from.rank][from.file] == KING_L)
        nextMove->position.castleConditions &= ~(Uint8)(WHITE_QUEENSIDE | WHITE_KINGSIDE);
    if (game->position.board[from.rank][from.file] == KING_D)
        nextMove->position.castleConditions &= ~(Uint8)(BLACK_QUEENSIDE | BLACK_KINGSIDE);

    // En passant
    nextMove->position.enPassant = move->enPassant;
    if (move->takesEnPassant) {
        nextMove->position.board[from.rank][to.file] = 0;
    }

    if (isCheck(&(nextMove->position))) {
        free(nextMove);
        return false;
    }

    nextMove->prev = game;
    nextMove->next = NULL;

    nextMove->position.turn = !nextMove->position.turn;

    game->next = nextMove;

    game = nextMove;

    freeMoves(possibleMoves);

    printf("%d\n", evalBoard(game->position.board));

    return true;
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