#include "../chess-project.h"

#include "rules.h"

#include <stdio.h>

// A szabályok definiálása

Pos RULE_KNIGHT[] = {
	{ 1, 2 },
	{ -1, 2 },
	{ 2, 1 },
	{ 2, -1 },
	{ 1, -2 },
	{ -1, -2 },
	{ -2, -1 },
	{ -2, 1 }
};

Pos RULE_BISHOP[] = {
	{ 1, 1 },
	{ -1, 1 },
	{ 1, -1 },
	{ -1, -1 }
};

Pos RULE_ROOK[] = {
	{ 0, 1 },
	{ 1, 0 },
	{ 0, -1 },
	{ -1, 0 }
};

Pos AddPos(Pos p1, Pos p2) {
	return (Pos) { p1.file + p2.file, p1.rank + p2.rank };
}

bool isInBound(Pos pos) {
	return 0 < pos.file && pos.file < 8 && 0 < pos.rank && pos.rank < 8;
}


bool addMove2(GamePosition* currentGamePos, Move2** moves, Pos move, bool color, Uint8 enpassant/* -1 if not */, bool takesEnpassant, Uint8 castle);
addMovesByRule(GamePosition* currentGamePos, Move2** moves, Pos rule[], int ruleCount, Pos position, bool color, bool repeat);


bool getColor(Uint8 piece) {
	return piece < KING_L ? BLACK : WHITE;
}

Move2* containsMove(Move2* moves, Pos move) {
	
	while (moves != NULL) {
		if (moves->pos.file == move.file && moves->pos.rank == move.rank) 
			return moves;
		moves = moves->next;
	}

	return NULL;
}

bool addMove2(GamePosition* currentGamePos,Move2** moves, Pos move, bool color, Uint8 enpassant/* -1 if not */, bool takesEnpassant, Uint8 castle) {

	if (move.file < 0 || move.file > 7 || move.rank < 0 || move.rank > 7)
		return false;

	Piece2 target = currentGamePos->board[move.rank][move.file];

	if(!(target == 0) && (getColor(target) == color) )
		return false;


	Move2 possibleMove = (Move2){ (Pos) { move.file,move.rank },enpassant, takesEnpassant,castle, NULL };
	Move2* newMove = malloc(sizeof(possibleMove));
	if (newMove == NULL) return false;
	*newMove = possibleMove;
	newMove->next = *moves;
	//newMove->enPassant = enpassant;
	//newMove->takesEnPassant = takesEnpassant;
	*moves = newMove;

	// return true if the target is empty
	return currentGamePos->board[move.rank][move.file] == 0;
}

addMovesByRule(GamePosition* currentGamePos,Move2** moves, Pos rule[], int ruleCount, Pos position, bool color, bool repeat) {
	for (int i = 0; i < ruleCount; i++) {
		Pos current = position;
		while (true) {
			current = AddPos(current, rule[i]);
			if (!addMove2(currentGamePos,moves,current,color,-1,false,0) || !repeat)
				break;
		}
	}
}

Move2* getPossibleMoves(GamePosition* currentGamePos, Pos selectedPos) {

	Move2* move = NULL;

	Uint8 piece = currentGamePos->board[selectedPos.rank][selectedPos.file];

	if (piece == 0) return move;

	bool color = getColor(piece);

	if (currentGamePos->turn != color) return move;

	switch (piece)
	{
		// TODO: pawn lépések, en passant
	case PAWN_L:
		// Just moving
		if (currentGamePos->board[selectedPos.rank - 1][selectedPos.file] == 0) {
			addMove2(currentGamePos, &move, (Pos) { selectedPos.file, selectedPos.rank - 1 }, color, -1,false,0);
			if (selectedPos.rank == 6 && currentGamePos->board[4][selectedPos.file] == 0) // Pawn a start pozícióban
				addMove2(currentGamePos, &move, (Pos) { selectedPos.file, selectedPos.rank - 2 }, color, selectedPos.file, false, 0);
		}

		// Takeing a piece
		if (currentGamePos->board[selectedPos.rank - 1][ selectedPos.file - 1] != 0)
			addMove2(currentGamePos, &move, (Pos) { selectedPos.file - 1, selectedPos.rank - 1 }, color, -1, false, 0);
		if (currentGamePos->board[selectedPos.rank - 1][ selectedPos.file + 1] != 0)
			addMove2(currentGamePos, &move, (Pos) { selectedPos.file + 1, selectedPos.rank - 1 }, color, -1, false, 0);


		
		// En Passante
		if (selectedPos.rank == 3) {
			if (currentGamePos->enPassant == selectedPos.file + 1)
				addMove2(currentGamePos, &move, (Pos) { selectedPos.file + 1, selectedPos.rank - 1 }, color, -1, true, 0);
			if (currentGamePos->enPassant == selectedPos.file - 1)
				addMove2(currentGamePos, &move, (Pos) { selectedPos.file - 1, selectedPos.rank - 1 }, color, -1, true, 0);
		}

		break;
	case PAWN_D:
		// Just moving
		if (currentGamePos->board[selectedPos.rank + 1][selectedPos.file] == 0) {
			addMove2(currentGamePos, &move, (Pos) { selectedPos.file, selectedPos.rank + 1 }, color, -1, false, 0);
			if (selectedPos.rank == 1 && currentGamePos->board[3][selectedPos.file] == 0) // Pawn a start pozícióban
				addMove2(currentGamePos, &move, (Pos) { selectedPos.file, selectedPos.rank + 2 }, color, selectedPos.file, false, 0);
		}

		// Takeing a piece
		if (currentGamePos->board[selectedPos.rank + 1][selectedPos.file - 1] != 0)
			addMove2(currentGamePos, &move, (Pos) { selectedPos.file - 1, selectedPos.rank + 1 }, color, -1, false, 0);
		if (currentGamePos->board[selectedPos.rank + 1][selectedPos.file + 1] != 0)
			addMove2(currentGamePos, &move, (Pos) { selectedPos.file + 1, selectedPos.rank + 1 }, color, -1, false, 0);

		// En Passante
		if (selectedPos.rank == 4) {
			if (currentGamePos->enPassant == selectedPos.file + 1)
				addMove2(currentGamePos, &move, (Pos) { selectedPos.file + 1, selectedPos.rank + 1 }, color, -1, true, 0);
			if (currentGamePos->enPassant == selectedPos.file - 1)
				addMove2(currentGamePos, &move, (Pos) { selectedPos.file - 1, selectedPos.rank + 1 }, color, -1, true, 0);
		}

		break;

	case BISHOP_D:
	case BISHOP_L:
		addMovesByRule(currentGamePos, &move, RULE_BISHOP, 4, (Pos) { selectedPos.file, selectedPos.rank }, color, true);
		break;

	case ROOK_D:
	case ROOK_L:
		addMovesByRule(currentGamePos, &move, RULE_ROOK, 4, (Pos) { selectedPos.file, selectedPos.rank }, color, true);
		break;

	case KNIGHT_D:
	case KNIGHT_L:
		addMovesByRule(currentGamePos, &move, RULE_KNIGHT, 8, (Pos) { selectedPos.file, selectedPos.rank }, color, false);
		break;

	case QUEEN_D:
	case QUEEN_L:
		addMovesByRule(currentGamePos, &move, RULE_ROOK, 4, (Pos) { selectedPos.file, selectedPos.rank }, color, true);
		addMovesByRule(currentGamePos, &move, RULE_BISHOP, 4, (Pos) { selectedPos.file, selectedPos.rank }, color, true);
		break;
	case KING_D:
	case KING_L:
		addMovesByRule(currentGamePos, &move, RULE_ROOK, 4, (Pos) { selectedPos.file, selectedPos.rank }, color, false);
		addMovesByRule(currentGamePos, &move, RULE_BISHOP, 4, (Pos) { selectedPos.file, selectedPos.rank }, color, false);

		// Castle

		if (!isCheck(currentGamePos)) {
			bool right = (currentGamePos->castleRights & (color ? WHITE_KINGSIDE : BLACK_KINGSIDE)) != 0;
			bool left = (currentGamePos->castleRights & (color ? WHITE_QUEENSIDE : BLACK_QUEENSIDE)) != 0;

			
			
			if(right && (currentGamePos->board[selectedPos.file + 1][selectedPos.rank] == 0) 
				&& !isAttacked(currentGamePos, (Pos) { selectedPos.file + 1, selectedPos.rank }, color))
				addMove2(currentGamePos, &move, (Pos) { selectedPos.file + 2, selectedPos.rank }, color, selectedPos.file, false, WHITE_KINGSIDE);
			if (left && (currentGamePos->board[selectedPos.file - 1][selectedPos.rank] == 0) 
				&& !isAttacked(currentGamePos, (Pos) { selectedPos.file - 1, selectedPos.rank }, color))
				addMove2(currentGamePos, &move, (Pos) { selectedPos.file - 2, selectedPos.rank }, color, selectedPos.file, false, WHITE_QUEENSIDE);

		}

		break;

	default:
		break;
	}
	// TODO: sakk tesztelése
	return move;
}

/* 
* #############################################
* #            Detecting checks               #
* #############################################
*/
bool scanPiece(GamePosition* currentGamePos, Pos rule[], int ruleCount, Pos king, bool repeat, Uint8 piece1, Uint8 piece2);

bool isCheck(GamePosition* position) {
	Pos king = { 0,0 };

	for (Uint8 file = 0; file < 8; file++) {
		for (Uint8 rank = 0; rank < 8; rank++) {
			if (position->board[rank][file] == (position->turn ? KING_L : KING_D)) {
				king.file = file; king.rank = rank;
				break;
			}
		}
	}

	return isAttacked(position, king, position->turn);
}

bool isAttacked(GamePosition* position, Pos coords, bool turn) {
	int dir = turn ? -1 : 1;
	Uint8 pawn = turn ? PAWN_D : PAWN_L;

	if (isInBound(AddPos(coords, (Pos) { 1, dir })))
		if ((position->board[coords.rank + dir][coords.file + 1] == pawn))
			return true;
	if (isInBound(AddPos(coords, (Pos) { -1, dir })))
		if ((position->board[coords.rank + dir][coords.file - 1] == pawn))
			return true;


	if (scanPiece(position, RULE_KNIGHT, 8, coords, false, position->turn ? KNIGHT_D : KNIGHT_L, -1))
		return true; // Attacked by knight

	if (scanPiece(position, RULE_BISHOP, 4, coords, true, position->turn ? BISHOP_D : BISHOP_L, position->turn ? QUEEN_D : QUEEN_L))
		return true; // Attacked by bishop or queen

	if (scanPiece(position, RULE_ROOK, 4, coords, true, position->turn ? ROOK_D : ROOK_L, position->turn ? QUEEN_D : QUEEN_L))
		return true; // Attacked by rook or queen

	// Test for king (this is necessary for testing possible moves)
	if (scanPiece(position, RULE_BISHOP, 4, coords, false, position->turn ? KING_D : KING_L, -1))
		return true; // Attacked by king

	if (scanPiece(position, RULE_ROOK, 4, coords, false, position->turn ? KING_D : KING_L, -1))
		return true; // Attacked by king


	return false;
}

bool scanPiece(GamePosition* currentGamePos, Pos rule[], int ruleCount, Pos king, bool repeat, Uint8 piece1, Uint8 piece2) {
	for (int i = 0; i < ruleCount; i++) {
		Pos current = king;
		while (true) {
			current = AddPos(current, rule[i]);
			
			if (current.rank < 0 || current.rank >= 8 || current.file < 0 || current.file >= 8) break;

			Uint8 currentPiece = currentGamePos->board[current.rank][current.file];

			if (currentPiece == piece1 || currentPiece == piece2) return true;
			
			if (currentPiece != 0 || !repeat) break;
		}
	}
	return false;
}

void freeMoves(Move2 * moves) {
	if (moves == NULL) return;
	freeMoves(moves->next);
	free(moves);
}