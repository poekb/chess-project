#include "../chess-project.h"

#include "move-eval.h"

void addMove(PossibleMove** moves, PossibleMove move) {
	PossibleMove* newMove = malloc(sizeof(PossibleMove));
	if (newMove == NULL) return;
	*newMove = move;
	newMove->next = *moves;
	*moves = newMove;
}

PossibleMove* getPossibleMoves(GamePosition gamePosition, Pos selectedPos) {
	// TODO: create this function

	PossibleMove* move = NULL;

	//addMove(&move, (PossibleMove) { selectedPos, NULL });

	Uint8 piece = gamePosition.board[selectedPos.rank][selectedPos.file];

	if (piece == 0) return move;


	switch (piece)
	{
	case PAWN_L:
		addMove(&move, (PossibleMove) { (Pos) { selectedPos.file,selectedPos.rank-1 }, NULL });

	default:
		break;
	}

	return move;
}

void clearMoves(PossibleMove* moves) {
	if (moves == NULL) return;
	clearMoves(moves->next);
	free(moves);
}