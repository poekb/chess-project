#include "../chess-project.h"

#include "move-eval.h"

// A szabályok definiálása

const Pos RULE_KNIGHT[] = {
	{ 1, 2 },
	{ -1, 2 },
	{ 2, 1 },
	{ 2, -1 },
	{ 1, -2 },
	{ -1, -2 },
	{ -2, -1 },
	{ -2, 1 }
};

const Pos RULE_BISHOP[] = {
	{ 1, 1 },
	{ -1, 1 },
	{ 1, -1 },
	{ -1, -1 }
};

const Pos RULE_ROOK[] = {
	{ 0, 1 },
	{ 1, 0 },
	{ 0, -1 },
	{ -1, 0 }
};

Pos AddPos(Pos p1, Pos p2) {
	return (Pos) { p1.file + p2.file, p1.rank + p2.rank };
}

GamePosition gamePosition;

Color getColor(Piece piece);
bool addMove(PossibleMove** moves, Pos move, Color color);
addMovesByRule(PossibleMove** moves, Pos rule[], int ruleCount, Pos position, Color color, bool repeat);


Color getColor(Piece piece) {
	return piece < KING_L ? BLACK : WHITE;
}

bool containsMove(PossibleMove* moves, Pos move) {
	
	while (moves != NULL) {
		if (moves->pos.file == move.file && moves->pos.rank == move.rank) 
			return true;
		moves = moves->next;
	}

	return false;
}

bool addMove(PossibleMove** moves, Pos move, Color color) {

	if (move.file < 0 || move.file > 7 || move.rank < 0 || move.rank > 7)
		return false;

	Piece target = gamePosition.board[move.rank][move.file];

	if(!(target == 0) && (getColor(target) == color) )
		return false;


	PossibleMove possibleMove = (PossibleMove){ (Pos) { move.file,move.rank }, NULL };
	PossibleMove* newMove = malloc(sizeof(possibleMove));
	if (newMove == NULL) return;
	*newMove = possibleMove;
	newMove->next = *moves;
	*moves = newMove;

	return gamePosition.board[move.rank][move.file] == 0;
}

addMovesByRule(PossibleMove** moves, Pos rule[], int ruleCount, Pos position, Color color, bool repeat) {
	for (int i = 0; i < ruleCount; i++) {
		Pos current = position;
		while (true) {
			current = AddPos(current, rule[i]);
			if (!addMove(moves,current,color) || !repeat)
				break;
		}
	}
}

PossibleMove* getPossibleMoves(GamePosition* currentGamePos, Pos selectedPos) {

	gamePosition = *currentGamePos;

	PossibleMove* move = NULL;

	Uint8 piece = gamePosition.board[selectedPos.rank][selectedPos.file];

	if (piece == 0) return move;

	Color color = getColor(piece);

	if (gamePosition.turn != color) return move;

	switch (piece)
	{
		// TODO: pawn lépések, en passant
	case PAWN_L:
		addMove(&move, (Pos){ selectedPos.file,selectedPos.rank - 1 }, color);
		break;
	case PAWN_D:
		addMove(&move, (Pos) { selectedPos.file, selectedPos.rank + 1 }, color);
		break;

	case BISHOP_D:
	case BISHOP_L:
		addMovesByRule(&move, RULE_BISHOP, 4, (Pos) { selectedPos.file, selectedPos.rank }, color, true);
		break;

	case ROOK_D:
	case ROOK_L:
		addMovesByRule(&move, RULE_ROOK, 4, (Pos) { selectedPos.file, selectedPos.rank }, color, true);
		break;

	case KNIGHT_D:
	case KNIGHT_L:
		addMovesByRule(&move, RULE_KNIGHT, 8, (Pos) { selectedPos.file, selectedPos.rank }, color, false);
		break;

	case QUEEN_D:
	case QUEEN_L:
		addMovesByRule(&move, RULE_ROOK, 4, (Pos) { selectedPos.file, selectedPos.rank }, color, true);
		addMovesByRule(&move, RULE_BISHOP, 4, (Pos) { selectedPos.file, selectedPos.rank }, color, true);
		break;
	case KING_D:
	case KING_L:
		addMovesByRule(&move, RULE_ROOK, 4, (Pos) { selectedPos.file, selectedPos.rank }, color, false);
		addMovesByRule(&move, RULE_BISHOP, 4, (Pos) { selectedPos.file, selectedPos.rank }, color, false);
		break;

	default:
		break;
	}
	// TODO: sakk tesztelése
	return move;
}

void clearMoves(PossibleMove* moves) {
	if (moves == NULL) return;
	clearMoves(moves->next);
	free(moves);
}