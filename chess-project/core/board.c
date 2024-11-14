#include "board.h"
#include "../zobrist/zobristHashing.h"

const int WhiteIndex = 0;
const int BlackIndex = 1;

void removePieceAtSquare(Board* board, Uint8 square, PieceType type, Uint8 colorIndex);

void movePiece(Board* board, Uint8 start, Uint8 target, PieceType type, bool turn);
void promotePiece(Board* board, Uint8 square, PieceType source, PieceType target, Uint8 colorIndex, bool isWhite);

/*
	Make a move on the board
	Also handle zobrist hash, and gamestate changes
*/
void makeMove(Board* board, Move move) {

	Uint8 start = getStart(move);
	Uint8 target = getTarget(move);

	Piece piece = board->square[start];
	bool turn = isWhite(piece);
	int colorIndex = turn ? WhiteIndex : BlackIndex;
	int enemyIndex = !turn ? WhiteIndex : BlackIndex;
	PieceType type = getPieceType(piece);


	bool enpassantCapture = isEnPassantCapture(move);
	bool castle = isCastle(move);
	bool doublePawn = isPawnTwoUp(move);
	bool promotion = isPromotion(move);

	board->zobristHistory[board->gameStateHistoryCount] = board->zobristHash;

	board->zobristHash ^= zobristCastlingRights[board->currentGameState.castleRights];

	GameState prevGameState = board->currentGameState;
	if (board->currentGameState.enpassantFile != (Uint8)-1)
		board->zobristHash ^= zobristEnpassant[board->currentGameState.enpassantFile];

	board->currentGameState.enpassantFile = -1;
	board->currentGameState.capturedPiece = enpassantCapture ? Pawn : getPieceType(board->square[target]);

	board->currentGameState.halfmoveClock++;

	if (type == Pawn)
		board->currentGameState.halfmoveClock = 0;

	// There is a capture
	if (board->currentGameState.capturedPiece != None || enpassantCapture) {
		board->currentGameState.halfmoveClock = 0;
		Uint8 capture = enpassantCapture ? (start / 8) * 8 + target % 8 : target;
		if (enpassantCapture)
			board->square[capture] = None;

		removePieceAtSquare(board, capture, board->currentGameState.capturedPiece, enemyIndex);
	}
	else if (doublePawn) {
		board->currentGameState.enpassantFile = start % 8;
		board->zobristHash ^= zobristEnpassant[board->currentGameState.enpassantFile];
	}
	movePiece(board, start, target, type, turn);

	// Handle castles
	if (castle) {
		if (board->kingSquare[colorIndex] % 8 == 6) {
			movePiece(board, target + 1, target - 1, Rook, turn); // Kingside
		}
		else if (board->kingSquare[colorIndex] % 8 == 2) {
			movePiece(board, target - 2, target + 1, Rook, turn); // Queenside
		}

		board->currentGameState.castleRights &= (turn ? ClearWhiteKingSide & ClearWhiteQueenSide : ClearBlackKingSide & ClearBlackQueenSide);
	}

	if (start == 0 || target == 0)
		board->currentGameState.castleRights &= ClearBlackQueenSide;
	else if (start == 7 || target == 7)
		board->currentGameState.castleRights &= ClearBlackKingSide;
	else if (start == 56 || target == 56)
		board->currentGameState.castleRights &= ClearWhiteQueenSide;
	else if (start == 63 || target == 63)
		board->currentGameState.castleRights &= ClearWhiteKingSide;
	
	board->zobristHash ^= zobristCastlingRights[board->currentGameState.castleRights];

	if (promotion) {
		switch (move >> 12) {
		case PromoteToQueenFlag:
			promotePiece(board, target, Pawn, Queen, colorIndex, turn);
			break;
		case PromoteToBishopFlag:
			promotePiece(board, target, Pawn, Bishop, colorIndex, turn);
			break;
		case PromoteToRookFlag:
			promotePiece(board, target, Pawn, Rook, colorIndex, turn);
			break;
		case PromoteToKnightFlag:
			promotePiece(board, target, Pawn, Knight, colorIndex, turn);
			break;
		}
	}
	
	board->isWhitesMove = !board->isWhitesMove;
	board->zobristHash ^= zobristBlacksTurn;

	if (board->isWhitesMove)
		board->fullmoveClock++;

	board->gameStateHistory[board->gameStateHistoryCount++] = prevGameState;
}

// Only get the zobrist hash from the resulting position of a move
Uint64 zobistOfMove(Board* board, Move move) {
	//Get data from move
	Uint8 start = getStart(move);
	Uint8 target = getTarget(move);

	Piece piece = board->square[start];
	bool turn = isWhite(piece);
	int colorIndex = turn ? WhiteIndex : BlackIndex;
	int enemyIndex = !turn ? WhiteIndex : BlackIndex;
	PieceType type = getPieceType(piece);


	bool enpassantCapture = isEnPassantCapture(move);
	bool castle = isCastle(move);
	bool doublePawn = isPawnTwoUp(move);
	bool promotion = isPromotion(move);

	Uint64 zobrist = board->zobristHash;

	GameState gamestate = board->currentGameState;
	zobrist ^= zobristCastlingRights[gamestate.castleRights];

	
	if (gamestate.enpassantFile != (Uint8)-1)
		zobrist ^= zobristEnpassant[gamestate.enpassantFile];

	gamestate.capturedPiece = enpassantCapture ? Pawn : getPieceType(board->square[target]);

	// There is a capture
	if (gamestate.capturedPiece != None || enpassantCapture) {
		Uint8 capture = enpassantCapture ? (start / 8) * 8 + target % 8 : target;
		zobrist ^= zobristPieceHashes[capture][(gamestate.capturedPiece - 1) + enemyIndex * 6];
	}
	else if (doublePawn) {
		gamestate.enpassantFile = start % 8;
		zobrist ^= zobristEnpassant[gamestate.enpassantFile];
	}

	zobrist ^= zobristPieceHashes[start][(type - 1) + colorIndex * 6];
	zobrist ^= zobristPieceHashes[target][(type - 1) + colorIndex * 6];

	// Handle castles
	if (castle) {
		if (target % 8 == 6) {
			// Kingside
			zobrist ^= zobristPieceHashes[target + 1][(Rook - 1) + colorIndex * 6];
			zobrist ^= zobristPieceHashes[target - 1][(Rook - 1) + colorIndex * 6];
		}
		else if (target % 8 == 2) {
			// Queenside
			zobrist ^= zobristPieceHashes[target - 2][(Rook - 1) + colorIndex * 6];
			zobrist ^= zobristPieceHashes[target + 1][(Rook - 1) + colorIndex * 6];
		}
	}

	if(type == King)
		gamestate.castleRights &= (turn ? ClearWhiteKingSide & ClearWhiteQueenSide : ClearBlackKingSide & ClearBlackQueenSide);

	if (start == 0 || target == 0)
		gamestate.castleRights &= ClearBlackQueenSide;
	else if (start == 7 || target == 7)
		gamestate.castleRights &= ClearBlackKingSide;
	else if (start == 56 || target == 56)
		gamestate.castleRights &= ClearWhiteQueenSide;
	else if (start == 63 || target == 63)
		gamestate.castleRights &= ClearWhiteKingSide;

	zobrist ^= zobristCastlingRights[gamestate.castleRights];

	if (promotion) {
		PieceType promoteType = Knight;
		switch (move >> 12) {
		case PromoteToQueenFlag:
			promoteType = Queen;
			break;
		case PromoteToBishopFlag:
			promoteType = Bishop;
			break;
		case PromoteToRookFlag:
			promoteType = Rook;
			break;
		}
		zobrist ^= zobristPieceHashes[target][(Pawn - 1) + colorIndex * 6];
		zobrist ^= zobristPieceHashes[target][(promoteType - 1) + colorIndex * 6];
	}

	zobrist ^= zobristBlacksTurn;
	return zobrist;
}

/*
	Complete inverse of the makeMove function
	Calling them one after the other should not make a meaningful change
*/ 
void unmakeMove(Board* board, Move move) {
	if (board->isWhitesMove)
		board->fullmoveClock--;
	board->isWhitesMove = !board->isWhitesMove;

	Uint8 start = getStart(move);
	Uint8 target = getTarget(move);

	Piece piece = board->square[target];
	bool turn = board->isWhitesMove;
	int colorIndex = turn ? WhiteIndex : BlackIndex;
	int enemyIndex = !turn ? WhiteIndex : BlackIndex;
	PieceType type = getPieceType(piece);

	bool enpassantCapture = isEnPassantCapture(move);
	bool castle = isCastle(move);
	bool doublePawn = isPawnTwoUp(move);
	bool promotion = isPromotion(move);

	movePiece(board, target, start, type, turn);

	if (promotion) {
		switch (move >> 12) {
		case PromoteToQueenFlag:
			promotePiece(board, start, Queen, Pawn, colorIndex, turn);
			break;
		case PromoteToBishopFlag:
			promotePiece(board, start, Bishop, Pawn, colorIndex, turn);
			break;
		case PromoteToRookFlag:
			promotePiece(board, start, Rook, Pawn, colorIndex, turn);
			break;
		case PromoteToKnightFlag:
			promotePiece(board, start, Knight, Pawn, colorIndex, turn);
			break;

		}
	}

	// There was a capture
	if (board->currentGameState.capturedPiece != None || enpassantCapture) {
		Uint8 capture = enpassantCapture ? (start / 8) * 8 + target % 8 : target;

		board->square[capture] = makePieceIsWhite(board->currentGameState.capturedPiece, !turn);

		makePieceAtSquare(board, capture, board->currentGameState.capturedPiece, enemyIndex);
	}

	if (castle) {
		if (target % 8 == 6) {
			movePiece(board, target - 1, target + 1, Rook, turn); // Kingside
		}
		else if (target % 8 == 2) {
			movePiece(board, target + 1, target - 2, Rook, turn); // Queenside
		}
	}
	board->currentGameState = board->gameStateHistory[--board->gameStateHistoryCount];
	board->zobristHash = board->zobristHistory[board->gameStateHistoryCount];
}



void promotePiece(Board* board, Uint8 square, PieceType source, PieceType target,Uint8 colorIndex, bool isWhite) {
	board->square[square] = makePieceIsWhite(target, isWhite);
	removePieceAtSquare(board, square, source, colorIndex);
	makePieceAtSquare(board, square, target, colorIndex);
}

void removePieceAtSquare(Board* board, Uint8 square, PieceType type, Uint8 colorIndex) {
	switch (type)
	{
	case Pawn:
		removePieceListAtSquare(&(board->Pawns[colorIndex]), square);
		break;
	case Knight:
		removePieceListAtSquare(&(board->Knights[colorIndex]), square);
		break;
	case Bishop:
		removePieceListAtSquare(&(board->Bishops[colorIndex]), square);
		break;
	case Rook:
		removePieceListAtSquare(&(board->Rooks[colorIndex]), square);
		break;
	case Queen:
		removePieceListAtSquare(&(board->Queens[colorIndex]), square);
		break;
	}

	board->zobristHash ^= zobristPieceHashes[square][(type - 1) + colorIndex * 6];
}

void makePieceAtSquare(Board* board, Uint8 square, PieceType type, Uint8 colorIndex) {
	
	switch (type)
	{
	case Pawn:
		addPieceListAtSquare(&(board->Pawns[colorIndex]), square);
		break;
	case Knight:
		addPieceListAtSquare(&(board->Knights[colorIndex]), square);
		break;
	case Bishop:
		addPieceListAtSquare(&(board->Bishops[colorIndex]), square);
		break;
	case Rook:
		addPieceListAtSquare(&(board->Rooks[colorIndex]), square);
		break;
	case Queen:
		addPieceListAtSquare(&(board->Queens[colorIndex]), square);
		break;
	case King:
		board->kingSquare[colorIndex] = square;
		break;
	}

	board->zobristHash ^= zobristPieceHashes[square][(type - 1) + colorIndex * 6];
}

void movePiece(Board* board, Uint8 start, Uint8 target, PieceType type, bool turn) {
	Uint8 colorIndex = turn ? WhiteIndex : BlackIndex;
	switch (type)
	{
	case Pawn:
		movePieceList(&(board->Pawns[colorIndex]), start, target);
		break;
	case Knight:
		movePieceList(&(board->Knights[colorIndex]), start, target);
		break;
	case Bishop:
		movePieceList(&(board->Bishops[colorIndex]), start, target);
		break;
	case Rook:
		movePieceList(&(board->Rooks[colorIndex]), start, target);
		break;
	case Queen:
		movePieceList(&(board->Queens[colorIndex]), start, target);
		break;
	case King:
		board->kingSquare[colorIndex] = target;
		board->currentGameState.castleRights &= (turn ? ClearWhiteKingSide & ClearWhiteQueenSide : ClearBlackKingSide & ClearBlackQueenSide); // Disable castling
		break;
	default:
		break;
	}
	board->zobristHash ^= zobristPieceHashes[start][(type - 1) + colorIndex * 6];
	board->zobristHash ^= zobristPieceHashes[target][(type - 1) + colorIndex * 6];

	board->square[target] = board->square[start];
	board->square[start] = None;
}

bool isCheckPos(Board* board) {
	return ((((Uint64)1 << board->kingSquare[board->isWhitesMove ? WhiteIndex : BlackIndex]) & board->underAttackMap) != 0);
}