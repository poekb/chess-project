#include "moveGenerator.h"
#include "precompution.h"
#include "../graphics/boardRenderer.h"

Uint64 pinMaps[64];
Uint64 pinnedPieces;

Uint64 checkersBitBoard;

Move* result;
int count;

bool onlyAttack;

Board* board;

// Appends a move to the possible moves
static void AddMove(Move move) {
	Uint8 target = GetTarget(move);
	Uint8 start = GetStart(move);
	
	if (IsEnPassantCapture(move)) {
		target = target % 8 + (start / 8) * 8;
	}
	if (onlyAttack && (board->square[target] == None)) return;

	if((checkersBitBoard & ((Uint64)1 << target)) != 0 && ((pinnedPieces & ((Uint64)1 << start)) == 0 || (pinMaps[start] & ((Uint64)1 << target)) != 0))
		result[count++] = move;
}

// Returns the number of possible moves
// and generates them at the specified address, resultIN
int GenerateMoves(Board* boardIn, Move* resultIn, bool onlyAttackIn) {
	onlyAttack = onlyAttackIn;
	board = boardIn;
	result = resultIn;
	count = 0;

	if (board->currentGameState.halfmoveClock >= 6) {
		if (board->currentGameState.halfmoveClock >= 100) {
			return 0; // 50 move rule
		}
		int sum = 0;
		for (int i = board->gameStateHistoryCount - board->currentGameState.halfmoveClock; i < board->gameStateHistoryCount; i+=2) {

			if (board->zobristHistory[i] == board->zobristHash)
				sum++;
		}

		if (sum >= 2) {
			return 0; // Draw by repetition
		}
	}
	
	bool turn = board->isWhitesMove;
	int colorIndex = turn ? WhiteIndex : BlackIndex;
	int enemyIndex = !turn ? WhiteIndex : BlackIndex;

	Uint64 underAttack = GenerateUnderAttackBitmap(board);
	board->underAttackMap = underAttack;

	Uint8 king = board->kingSquare[colorIndex];

	checkersBitBoard = UINT64_MAX;

	for (int i = 0; i < knightMovesCount[king]; i++) {
		if (board->square[knightMoves[king][i]] == MakePieceIsWhite(Knight, !turn)) {
			checkersBitBoard &= ((Uint64) 1 << knightMoves[king][i]);
			break;
		}
	}

	for (int i = 0; i < (turn ? whitePawnAttacksCount[king] : blackPawnAttacksCount[king]); i++) {

		Uint8 target = turn ? whitePawnAttacks[king][i] : blackPawnAttacks[king][i];
		if (board->square[target] == MakePieceIsWhite(Pawn, !turn)) {
			checkersBitBoard &= ((Uint64)1 << target);
			break;
		}
	}

	// Sliders
	pinnedPieces = 0;

	for (int d = 0; d < 8; d++) {
		Uint8 pinnedPiece = -1;
		Uint64 pinMap = 0;

		bool attacked = false;

		Uint8 current = king;
		
		for (int i = 0; i < NumOfSquaresFromEdge[king][d]; i++) {
			current += Directions[d];
			
			pinMap |= ((Uint64)1 << current);
			
			Piece piece = board->square[current];
			if (piece != None) {
				if (IsWhite(piece) == turn) {
					if(pinnedPiece == 255)
						pinnedPiece = current;
					else {
						break;
					}
				}
				else {
					PieceType type = GetPieceType(piece);
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

			if ((board->square[index] == None || IsWhite(board->square[index]) != turn) && (((Uint64)1 << index) & underAttack) == 0)
				result[count++] = GetMove(king, index);
		}
	}
	

	if (checkersBitBoard == 0) return count; // there is no moves left

	// Knights
	PieceList* Knights = &(board->Knights[colorIndex]);
	for (int i = 0; i < Knights->count; i++) {
		Uint8 index = Knights->list[i];

		for (int j = 0; j < knightMovesCount[index]; j++) {
			Uint8 target = knightMoves[index][j];
			if (board->square[target] == None || IsWhite(board->square[target]) != turn)
				AddMove(GetMove(index, target));
		}
	}

	// Bishops
	PieceList* Bishops = &(board->Bishops[colorIndex]);
	for (int i = 0; i < Bishops->count; i++) {
		Uint8 index = Bishops->list[i];
		for (int d = 4; d < 8; d++) {
			Uint8 current = index;

			for (int j = 0; j < NumOfSquaresFromEdge[index][d]; j++)
			{
				current += Directions[d];

				if (board->square[current] != None) {
					if (IsWhite(board->square[current]) != turn)
						AddMove(GetMove(index, current));
					break;
				}
				AddMove(GetMove(index, current));
			}
		}
	}

	// Rooks
	PieceList* Rooks = &(board->Rooks[colorIndex]);
	for (int i = 0; i < Rooks->count; i++) {
		Uint8 index = Rooks->list[i];
		for (int d = 0; d < 4; d++) {
			Uint8 current = index;

			for (int j = 0; j < NumOfSquaresFromEdge[index][d]; j++)
			{
				current += Directions[d];

				if (board->square[current] != None) {
					if (IsWhite(board->square[current]) != turn)
						AddMove(GetMove(index, current));
					break;
				}
				AddMove(GetMove(index, current));
			}
		}
	}

	// Queens
	PieceList* Queens = &(board->Queens[colorIndex]);
	for (int i = 0; i < Queens->count; i++) {
		Uint8 index = Queens->list[i];
		for (int d = 0; d < 8; d++) {
			Uint8 current = index;

			for (int j = 0; j < NumOfSquaresFromEdge[index][d]; j++)
			{
				current += Directions[d];

				if (board->square[current] != None) {
					if (IsWhite(board->square[current]) != turn)
						AddMove(GetMove(index, current));
					break;
				}
				AddMove(GetMove(index, current));
			}
		}
	}

	if ((underAttack & ((Uint64)1 << king)) == 0 && !onlyAttack) {

		// Castling
		bool kingSide = HasKingsideCastleRight(board->currentGameState, turn);
		bool queenSide = HasQueensideCastleRight(board->currentGameState, turn);
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
				AddMove(GetMove(king, king + 2) | (CastleFlag << 12));
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
				AddMove(GetMove(king, king - 2) | (CastleFlag << 12));
		}

	}
	// Pawns
	PieceList* Pawns = &(board->Pawns[colorIndex]);
	for (int i = 0; i < Pawns->count; i++) {
		Uint8 index = Pawns->list[i];
		Uint8 rank = index / 8;
		bool IsPromotion = (rank == (turn ? 1 : 6));
		if (board->square[index + (turn ? -8 : 8)] == None) {
			Move move = GetMove(index, index + (turn ? -8 : 8));
			if (IsPromotion) {
				AddMove(move | (PromoteToQueenFlag << 12));
				AddMove(move | (PromoteToBishopFlag << 12));
				AddMove(move | (PromoteToRookFlag << 12));
				AddMove(move | (PromoteToKnightFlag << 12));
			}
			else {
				AddMove(move);
			}
			if ((rank == (turn ? 6 : 1)) && (board->square[index + (turn ? -16 : 16)] == None))
			{
				AddMove((GetMove(index, index + (turn ? -16 : 16)) | (PawnTwoUpFlag << 12)));
			}

		}

		bool enPassant = (board->currentGameState.enpassantFile != (Uint8)-1) && rank == (turn ? 3 : 4);

		for (int i = 0; i < (turn ? whitePawnAttacksCount[index] : blackPawnAttacksCount[index]); i++) {
			Uint8 target = (turn ? whitePawnAttacks[index][i] : blackPawnAttacks[index][i]);

			if (enPassant && (board->currentGameState.enpassantFile == (target % 8))) {
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
						PieceType type = GetPieceType(piece);
						if (piece != None) {
							if (IsWhite(piece) != turn && (type == Rook || type == Queen))
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
						PieceType type = GetPieceType(piece);
						if (piece != None) {
							if (IsWhite(piece) != turn && (type == Rook || type == Queen))
								isHorizontal = true;
							break;
						}
					}
					if(!(isKing && isHorizontal))
						AddMove(GetMove(index, target) | EnPassantCaptureFlag << 12);
					
				}else AddMove(GetMove(index, target) | EnPassantCaptureFlag << 12);
			}
			else if (board->square[target] != None && IsWhite(board->square[target]) != turn) {

				Move move = GetMove(index, target);
				if (IsPromotion) {
					AddMove(move | (PromoteToQueenFlag << 12));
					AddMove(move | (PromoteToBishopFlag << 12));
					AddMove(move | (PromoteToRookFlag << 12));
					AddMove(move | (PromoteToKnightFlag << 12));
				}
				else {
					AddMove(move);
				}
			}
		}
	}

	return count;
}

// Generate what squares the enemy pieces can see, represented by a 64 bit bitmap
Uint64 GenerateUnderAttackBitmap(Board* board) {
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

			for (int j = 0; j < NumOfSquaresFromEdge[index][d]; j++)
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

			for (int j = 0; j < NumOfSquaresFromEdge[index][d]; j++)
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

			for (int j = 0; j < NumOfSquaresFromEdge[index][d]; j++)
			{
				current += Directions[d];
				result |= (Uint64)1 << current;
				if (board->square[current] != None && current != board->kingSquare[1 - colorIndex]) break;
			}
		}
	}

	return result;
}