#include "moveGenerator.h"
#include "precompution.h"
#include "../graphics/board-renderer.h"

Uint64 pinMaps[64];
Uint64 pinnedPieces;

Uint64 checkersBitBoard;

Move* result;
int count;

bool onlyAttack;

Board* board;

static void addMove(Move move) {
	Uint8 target = getTarget(move);
	if (onlyAttack && (
		!isEnPassantCapture(move) && (board->square[target] == None)
		)) return;
	Uint8 start = getStart(move);
	if((checkersBitBoard & ((Uint64)1 << target)) != 0 && ((pinnedPieces & ((Uint64)1 << start)) == 0 || (pinMaps[start] & ((Uint64)1 << target)) != 0))
		result[count++] = move;
}

// Returns the number of possible moves
int generateMoves(Board* boardIn, Move* resultIn, bool onlyAttackIn) {
	onlyAttack = onlyAttackIn;
	board = boardIn;
	result = resultIn;
	count = 0;

	bool turn = board->isWhitesMove;
	int colorIndex = turn ? WhiteIndex : BlackIndex;
	int enemyIndex = !turn ? WhiteIndex : BlackIndex;

	Uint64 underAttack = generateUnderAttackBitmap(board);
	board->underAttackMap = underAttack;
	//displayBitboard(renderer, underAttack, (SDL_Color) { 100, 0, 0, 200 });

	Uint8 king = board->kingSquare[colorIndex];

	checkersBitBoard = UINT64_MAX;

	// TODO: optimize with bitmaps

	for (int i = 0; i < knightMovesCount[king]; i++) {
		if (board->square[knightMoves[king][i]] == makePieceIsWhite(Knight, !turn)) {
			checkersBitBoard &= ((Uint64) 1 << knightMoves[king][i]);
			break;
		}
	}
	//highlightCells(renderer, blackPawnAttacks[king], blackPawnAttacksCount[king], (SDL_Color) { 0, 0, 200, 200 });
	for (int i = 0; i < (turn ? whitePawnAttacksCount[king] : blackPawnAttacksCount[king]); i++) {

		Uint8 target = turn ? whitePawnAttacks[king][i] : blackPawnAttacks[king][i];
		if (board->square[target] == makePieceIsWhite(Pawn, !turn)) {
			checkersBitBoard &= ((Uint64)1 << target);
			break;
		}
	}

	//displayBitboard(renderer, checkersBitBoard, (SDL_Color) { 0, 200, 0, 100 });

	// Sliders
	pinnedPieces = 0;

	for (int d = 0; d < 8; d++) {
		Uint8 pinnedPiece = -1;
		Uint64 pinMap = 0;

		bool attacked = false;

		Uint8 current = king;
		
		for (int i = 0; i < NumSquaresFromEdge[king][d]; i++) {
			current += Directions[d];
			
			pinMap |= ((Uint64)1 << current);
			
			Piece piece = board->square[current];
			if (piece != None) {
				if (isWhite(piece) == turn) {
					if(pinnedPiece == 255)
						pinnedPiece = current;
					else {
						break;
					}
				}
				else {
					PieceType type = getPieceType(piece);
					if (type == Queen || type == ((d < 4) ? Rook : Bishop)) {
						if (pinnedPiece == 255) {
							checkersBitBoard &= pinMap;
						}
						else {
							pinnedPieces |= ((Uint64)1 << pinnedPiece);
							pinMaps[pinnedPiece] = pinMap;
							attacked = true;
						}
						break;
					}else if (pinnedPiece == 255)
						pinnedPiece = current;
					else {
						break;
					}
				}
			}
		}
		if (checkersBitBoard == 0) {
			break;
		}
	}

	// King
	if (!onlyAttack) {
		for (int i = 0; i < kingMovesCount[king]; i++) {
			Uint8 index = kingMoves[king][i];

			if ((board->square[index] == None || isWhite(board->square[index]) != turn) && (((Uint64)1 << index) & underAttack) == 0)
				result[count++] = getMove(king, index);
		}
	}
	

	if (checkersBitBoard == 0) return count; // there is no moves left

	// Knights
	PieceList* Knights = &(board->Knights[colorIndex]);
	for (int i = 0; i < Knights->count; i++) {
		Uint8 index = Knights->list[i];

		for (int j = 0; j < knightMovesCount[index]; j++) {
			Uint8 target = knightMoves[index][j];
			if (board->square[target] == None || isWhite(board->square[target]) != turn)
				addMove(getMove(index, target));
		}
	}

	// Bishops
	PieceList* Bishops = &(board->Bishops[colorIndex]);
	for (int i = 0; i < Bishops->count; i++) {
		Uint8 index = Bishops->list[i];
		for (int d = 4; d < 8; d++) {
			Uint8 current = index;

			for (int j = 0; j < NumSquaresFromEdge[index][d]; j++)
			{
				current += Directions[d];

				if (board->square[current] != None) {
					if (isWhite(board->square[current]) != turn)
						addMove(getMove(index, current));
					break;
				}
				addMove(getMove(index, current));
			}
		}
	}

	// Rooks
	PieceList* Rooks = &(board->Rooks[colorIndex]);
	for (int i = 0; i < Rooks->count; i++) {
		Uint8 index = Rooks->list[i];
		for (int d = 0; d < 4; d++) {
			Uint8 current = index;

			for (int j = 0; j < NumSquaresFromEdge[index][d]; j++)
			{
				current += Directions[d];

				if (board->square[current] != None) {
					if (isWhite(board->square[current]) != turn)
						addMove(getMove(index, current));
					break;
				}
				addMove(getMove(index, current));
			}
		}
	}


	// Queens
	PieceList* Queens = &(board->Queens[colorIndex]);
	for (int i = 0; i < Queens->count; i++) {
		Uint8 index = Queens->list[i];
		for (int d = 0; d < 8; d++) {
			Uint8 current = index;

			for (int j = 0; j < NumSquaresFromEdge[index][d]; j++)
			{
				current += Directions[d];

				if (board->square[current] != None) {
					if (isWhite(board->square[current]) != turn)
						addMove(getMove(index, current));
					break;
				}
				addMove(getMove(index, current));
			}
		}
	}

	if ((underAttack & ((Uint64)1 << king)) == 0 && !onlyAttack) {

		// Castling
		bool kingSide = hasKingsideCastleRight(board->currentGameState, turn);
		bool queenSide = hasQueensideCastleRight(board->currentGameState, turn);
		if (kingSide) {
			bool valid = true;
			for (int j = 1; j <= 2; j++) {
				if (board->square[king + j] != None || (((Uint64)1 << (king + j)) & underAttack) != 0)
				{
					valid = false;
					break;
				}
			}

			if (valid)
				addMove(getMove(king, king + 2) | (CastleFlag << 12));
		}

		if (queenSide) {
			bool valid = true;
			for (int j = 1; j <= 3; j++) {
				if (board->square[king - j] != None || ((((Uint64)1 << (king - j)) & underAttack) != 0 && j != 3))
				{
					valid = false;
					break;
				}
			}

			if (valid)
				addMove(getMove(king, king - 2) | (CastleFlag << 12));
		}

	}
	// Pawns
	PieceList* Pawns = &(board->Pawns[colorIndex]);
	for (int i = 0; i < Pawns->count; i++) {
		Uint8 index = Pawns->list[i];
		Uint8 rank = index / 8;
		bool isPromotion = (rank == (turn ? 1 : 6));
		if (board->square[index + (turn ? -8 : 8)] == None) {
			addMove(getMove(index, index + (turn ? -8 : 8)) | (isPromotion ? PromoteToQueenFlag << 12 : 0));
			if ((rank == (turn ? 6 : 1)) && (board->square[index + (turn ? -16 : 16)] == None))
			{
				addMove((getMove(index, index + (turn ? -16 : 16)) | (PawnTwoUpFlag << 12)));
			}

		}

		bool enPassant = board->currentGameState.enpassantFile != 0 && rank == (turn ? 3 : 4);

		for (int i = 0; i < (turn ? whitePawnAttacksCount[index] : blackPawnAttacksCount[index]); i++) {
			Uint8 target = (turn ? whitePawnAttacks[index][i] : blackPawnAttacks[index][i]);

			if (enPassant && ((board->currentGameState.enpassantFile & (1 << (target % 8))) != 0)) {
				// TODO: test for checks

				if (king / 8 == rank) {
					Uint8 left = min(target % 8, index % 8);

					bool isKing = false;
					bool isHorizontal = false;
					for (int x = left - 1; x >= 0; x--) {
						Uint8 current = rank * 8 + x;
						if (king == current) {
							isKing = true;
							break;
						}
						Piece piece = board->square[current];
						PieceType type = getPieceType(piece);
						if (piece != None) {
							if (isWhite(piece) != turn && (type == Rook || type == Queen))
								isHorizontal = true;
							break;
						}					
					}
					for (int x = left + 2; x < 8; x++) {
						Uint8 current = rank * 8 + x;
						if (king == current) {
							isKing = true;
							break;
						}
						Piece piece = board->square[current];
						PieceType type = getPieceType(piece);
						if (piece != None) {
							if (isWhite(piece) != turn && (type == Rook || type == Queen))
								isHorizontal = true;
							break;
						}
					}
					if(!(isKing && isHorizontal))
						addMove(getMove(index, target) | EnPassantCaptureFlag << 12);
					
				}else addMove(getMove(index, target) | EnPassantCaptureFlag << 12);
			}
			else if (board->square[target] != None && isWhite(board->square[target]) != turn) {
				addMove((getMove(index, target)) | (isPromotion ? PromoteToQueenFlag << 12 : 0));
			}

		}

	}

	return count;
}




Uint64 generateUnderAttackBitmap(Board* board) {
	Uint64 result = 0;

	bool turn = board->isWhitesMove;
	int colorIndex = !turn ? WhiteIndex : BlackIndex;

	// Pawns
	PieceList* Pawns = &(board->Pawns[colorIndex]);
	for (int i = 0; i < Pawns->count; i++) {
		result |= pawnAttackBitBoards[Pawns->list[i]][colorIndex];
	}

	// Knights
	PieceList* Knights = &(board->Knights[colorIndex]);
	for (int i = 0; i < Knights->count; i++) {
		result |= knightAttackBitBoards[Knights->list[i]];
	}

	// King
	Uint8 king = board->kingSquare[colorIndex];
	result |= kingAttackBitBoards[king];

	// Bishops
	PieceList* Bishops = &(board->Bishops[colorIndex]);
	for (int i = 0; i < Bishops->count; i++) {
		Uint8 index = Bishops->list[i];
		for (int d = 4; d < 8; d++) {
			Uint8 current = index;

			for (int j = 0; j < NumSquaresFromEdge[index][d]; j++)
			{
				current += Directions[d];
				result |= (Uint64)1 << current;
				if (board->square[current] != None && current != board->kingSquare[1-colorIndex]) break;
			}
		}
	}

	// Rooks
	PieceList* Rooks = &(board->Rooks[colorIndex]);
	for (int i = 0; i < Rooks->count; i++) {
		Uint8 index = Rooks->list[i];
		for (int d = 0; d < 4; d++) {
			Uint8 current = index;

			for (int j = 0; j < NumSquaresFromEdge[index][d]; j++)
			{
				current += Directions[d];
				result |= (Uint64)1 << current;
				if (board->square[current] != None && current != board->kingSquare[1 - colorIndex]) break;
			}
		}
	}

	// Queens
	PieceList* Queens = &(board->Queens[colorIndex]);
	for (int i = 0; i < Queens->count; i++) {
		Uint8 index = Queens->list[i];
		for (int d = 0; d < 8; d++) {
			Uint8 current = index;

			for (int j = 0; j < NumSquaresFromEdge[index][d]; j++)
			{
				current += Directions[d];
				result |= (Uint64)1 << current;
				if (board->square[current] != None && current != board->kingSquare[1 - colorIndex]) break;
			}
		}
	}

	return result;
}