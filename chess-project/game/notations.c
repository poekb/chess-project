#include "notations.h"
#include "../moveGeneration/moveGenerator.h"

int getMoveNotation(Board* board, Move move, char* string) {
	int length = 0;
	Move* moves = malloc(sizeof(Move) * 100);
	if (moves == NULL) return 0;
	int moveCount = generateMoves(board, moves, false);

	Uint8 start = getStart(move);
	Uint8 target = getTarget(move);

	PieceType type = getPieceType(board->square[start]);

	if (isCastle(move)) {

		string[length++] = '0';
		string[length++] = '-';
		string[length++] = '0';

		if (target % 8 == 2) // Queen side
		{
			string[length++] = '-';
			string[length++] = '0';
		}

		return length;
	}

	switch (type)
	{
	case(King):
		string[length++] = 'K';
		break;
	case(Queen):
		string[length++] = 'Q';
		break;
	case(Rook):
		string[length++] = 'R';
		break;
	case(Bishop):
		string[length++] = 'B';
		break;
	case(Knight):
		string[length++] = 'N';
		break;
	}

	bool capture = board->square[target] != None || isEnPassantCapture(move);

	bool multiple = (capture) && (type == Pawn);
	bool sameFile = false;
	bool sameRank = false;

	for (int i = 0; i < moveCount; i++) {
		if (move == moves[i]) continue;
		Uint8 curStart = getStart(moves[i]);

		if (target != getTarget(moves[i]) || getPieceType(board->square[curStart]) != type)
			continue;
		multiple = true;
		if (start % 8 == curStart % 8)
			sameFile = true;
		else if (start / 8 == curStart / 8)
			sameRank = true;
	}

	if (multiple) {
		if (!sameFile)
		{
			string[length++] = (start % 8) + 'a';
		}
		else if (!sameRank) {
			string[length++] = '8' - (start / 8);
		}
		else {
			string[length++] = (start % 8) + 'a';
			string[length++] = '8' - (start / 8);
		}
	}

	if (capture) {
		string[length++] = 'x';
		
	}

	string[length++] = (target % 8) + 'a';
	string[length++] = '8' - (target / 8);

	if (isPromotion(move)) {
		string[length++] = '='; //???

		if (move >> 12 == PromoteToQueenFlag) {
			string[length++] = 'Q';
		}
		else if (move >> 12 == PromoteToRookFlag) {
			string[length++] = 'R';
		}
		else if (move >> 12 == PromoteToBishopFlag) {
			string[length++] = 'B';
		}
		else if (move >> 12 == PromoteToKnightFlag) {
			string[length++] = 'N';
		}
	}

	MakeMove(board, move);
	moveCount = generateMoves(board, moves, false);
	bool isCheck = isCheckPos(board);
	RevokeMove(board, move);

	if (isCheck) {
		if (moveCount == 0) {
			string[length++] = '#';
		}
		else {
			string[length++] = '+';
		}
	}

	free(moves);

	return length;
}

void getFENFromBoard(Board* board, char* FEN) {
	int length = 0;	

	for (int i = 0; i < 8; i++) {
		int sum = 0;
		for (int j = 0; j < 8; j++) {
			Piece piece = board->square[i * 8 + j];
			if (piece == None) {
				sum++;
			}
			else {
				if (sum != 0)
					FEN[length++] = ('0' + sum);
				sum = 0;
				FEN[length++] = charFromPiece(piece);
			}
			
		}
		if (sum != 0)
			FEN[length++] = ('0' + sum);
		if(i < 7)
			FEN[length++] = '/';
	}
	FEN[length++] = ' ';
	FEN[length++] = (board->isWhitesMove ? 'w' : 'b');
	FEN[length++] = ' ';

	Uint8 castleRights = board->currentGameState.castleRights;
	if (castleRights > 0) {
		if ((castleRights & WhiteKingSide) != 0) FEN[length++] = 'K';
		if ((castleRights & WhiteQueenSide) != 0) FEN[length++] = 'Q';
		if ((castleRights & BlackKingSide) != 0) FEN[length++] = 'k';
		if ((castleRights & BlackQueenSide) != 0) FEN[length++] = 'q';
	}
	else {
		FEN[length++] = '-';
	}

	FEN[length++] = ' ';

	Uint8 enPassantFile = board->currentGameState.enpassantFile;
	if (enPassantFile == 0) {
		FEN[length++] = '-';
	}
	else {
		char file = 'a' - 1;
		while (enPassantFile != 0) {
			enPassantFile >>= 1;
			file++;
		}
		FEN[length++] = file;
	}


	int result = sprintf(&FEN[length], " %d %d", board->currentGameState.halfmoveClock, board->fullmoveClock);
	length += result;

	FEN[length] = '\0';

	return length;
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
				board->currentGameState.castleRights |= BlackKingSide;
				break;
			case 'q':
				board->currentGameState.castleRights |= BlackQueenSide;
				break;
			case 'K':
				board->currentGameState.castleRights |= WhiteKingSide;
				break;
			case 'Q':
				board->currentGameState.castleRights |= WhiteQueenSide;
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

	strPointer++;
	int halfMove, fullMove;
	int result = sscanf(&FENString[strPointer], "%d %d", &halfMove, &fullMove);

	if (result < 2) {
		halfMove = 0;
		fullMove = 1;
	}

	board->currentGameState.halfmoveClock = (Uint16)halfMove;
	board->fullmoveClock = (Uint16)fullMove;
	

	board->gameStateHistoryCount = 0;

}