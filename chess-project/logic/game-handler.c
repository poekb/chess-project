#include "game-handler.h"

#include "move-eval.h"
#include "../graphics/board-renderer.h"

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


    // En passant
    nextMove->position.enPassant = move->enPassant;
    if (move->takesEnPassant) {
        nextMove->position.board[from.rank][to.file] = 0;
    }

    if (isCheck(nextMove)) {
        free(nextMove);
        return false;
    }

    nextMove->prev = game;
    nextMove->next = NULL;

    nextMove->position.turn = !nextMove->position.turn;

    game->next = nextMove;

    game = nextMove;

    freeMoves(possibleMoves);

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