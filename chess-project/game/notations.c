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
	char FEN2[100];
	

	for (int i = 0; i < 8; i++) {
		int sum = 0;
		for (int j = 0; j < 8; j++) {
			Piece piece = board->square[i * 8 + j];
			if (piece == None) {
				sum++;
			}
			else {
				if (sum != 0)
					FEN2[length++] = ('0' + sum);
				sum = 0;
				FEN2[length++] = charFromPiece(piece);
			}
			
		}
		if (sum != 0)
			FEN2[length++] = ('0' + sum);
		if(i < 7)
			FEN2[length++] = '/';
	}
	FEN2[length++] = ' ';


	FEN2[length] = '\0';

	printf("%s\n",FEN2);
}