#include "board.h"

const int WhiteIndex = 0;
const int BlackIndex = 1;

void makePieceAtSquare(Board* board, Uint8 square, PieceType type, Uint8 colorIndex);
void removePieceAtSquare(Board* board, Uint8 square, PieceType type, Uint8 colorIndex);

void movePiece(Board* board, Uint8 start, Uint8 target, PieceType type, bool turn);
void promotePiece(Board* board, Uint8 square, PieceType source, PieceType target, Uint8 colorIndex, bool isWhite);


void MakeMove(Board* board, Move move) {

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

	GameState prevGameState = board->currentGameState;
	board->currentGameState.enpassantFile = 0;
	board->currentGameState.capturedPiece = enpassantCapture ? Pawn : getPieceType(board->square[target]);

	// There is a capture
	if (board->currentGameState.capturedPiece != None || enpassantCapture) {
		Uint8 capture = enpassantCapture ? (start / 8)*8 + target % 8 : target;
		if (enpassantCapture)
			board->square[capture] = None;

		removePieceAtSquare(board, capture, board->currentGameState.capturedPiece, enemyIndex);
	}
	else if (doublePawn) {
		board->currentGameState.enpassantFile = (Uint8)1 << start % 8;
	}

	movePiece(board, start, target, type, turn);

	// Handle castles
	if (castle) {
		if (board->kingSquare[colorIndex] % 8 == 6) {
			movePiece(board, target + 1, target - 1, Rook, turn); // Kingside
		}
		if (board->kingSquare[colorIndex] % 8 == 2) {
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
	

	if (promotion) {
		if (move >> 12 == PromoteToQueenFlag) {
			promotePiece(board, target, Pawn, Queen, colorIndex, turn);
		}
	}

	board->isWhitesMove = !board->isWhitesMove;

	board->gameStateHistory[board->gameStateHistoryCount++] = prevGameState;

}


void RevokeMove(Board* board, Move move) {

	board->isWhitesMove = !board->isWhitesMove;

	//Get data from move
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
		if (move >> 12 == PromoteToQueenFlag) {
			promotePiece(board, start, Queen, Pawn, colorIndex, turn);
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
		if (target % 8 == 2) {
			movePiece(board, target + 1, target - 2, Rook, turn); // Queenside
		}
	}


	board->currentGameState = board->gameStateHistory[--board->gameStateHistoryCount];
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
	}
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
	board->square[target] = board->square[start];
	board->square[start] = None;
}


void LoadBoardFromFEN(Board* board, char* FENString) {
	int strPointer = 0;
	int pointer = 0;
	while (pointer < 64) {
		char c = FENString[strPointer];
		if ('0' < c && c < '9') {
			for (int i = 1; i < c - '0'; i++) {
				board->square[pointer++] = None;
			}
			pointer++;
		}
		else if (c != '/') {
			Piece piece = pieceFromChar(c);
			board->square[pointer] = piece;
			int colorIndex = isWhite(piece) ? WhiteIndex : BlackIndex;
			PieceType type = getPieceType(piece);
			switch (type)
			{
			case Pawn:
				addPieceListAtSquare(&(board->Pawns[colorIndex]), pointer);
				break;
			case Knight:
				addPieceListAtSquare(&(board->Knights[colorIndex]), pointer);
				break;
			case Bishop:
				addPieceListAtSquare(&(board->Bishops[colorIndex]), pointer);
				break;
			case Rook:
				addPieceListAtSquare(&(board->Rooks[colorIndex]), pointer);
				break;
			case Queen:
				addPieceListAtSquare(&(board->Queens[colorIndex]), pointer);
				break;
			case King:
				board->kingSquare[colorIndex] = pointer;
			default:
				break;
			}
			pointer++;
		}
		strPointer++;
	}

	strPointer++;
	board->isWhitesMove = (FENString[strPointer] == 'w');
	board->currentGameState.capturedPiece = None;
	strPointer++;

	board->currentGameState.castleRights = 0;

	if (FENString[strPointer] != '-') {
		while (FENString[++strPointer] != ' ') {
			switch (FENString[strPointer]) {
			case 'k':
				board->currentGameState.castleRights |= 0b1000;
				break;
			case 'q':
				board->currentGameState.castleRights |= 0b0100;
				break;
			case 'K':
				board->currentGameState.castleRights |= 0b0010;
				break;
			case 'Q':
				board->currentGameState.castleRights |= 0b0001;
				break;
			default: break;
			}
		}
	}


	strPointer++;

	if (FENString[strPointer] != '-')
		board->currentGameState.enpassantFile = ((Uint8)1 << (FENString[strPointer] - 'a'));
	else
		board->currentGameState.enpassantFile = 0;

	board->gameStateHistoryCount = 0;

}