#include "pgn.h"

#include "gameHandler.h"
#include "notations.h"

char* getPGN(Board* board) {

	int moveCount = 0;
	while (prevEnabled) {
		prevMove();
		moveCount++;
	}

	char* pgn = malloc(sizeof(char) * (130 + 12 * moveCount));

	if (pgn == NULL) {
		SDL_Log("Unable to allocate memory");
		exit(1);
	}

	int offset = sprintf(pgn, "[FEN \"%s\"]\n\n", startFEN);

	if (moveCount == 0) {
		return pgn;
	}

	if (!board->isWhitesMove) {
		char* blackMove = getMoveNotation(board, getNextMove());
		offset += sprintf_s(&pgn[offset], 24, "%d... %s ", board->fullmoveClock, blackMove);
		nextMove();
		free(blackMove);
		moveCount--;
	}

	while (moveCount >= 2) {
		char* whiteMove = getMoveNotation(board, getNextMove());
		nextMove();
		char* blackMove = getMoveNotation(board, getNextMove());
		offset += sprintf_s(&pgn[offset], 24, "%d. %s %s ", board->fullmoveClock, whiteMove, blackMove);
		nextMove();
		free(whiteMove);
		free(blackMove);
		moveCount -= 2;
	}

	if (moveCount == 1) {
		char* whiteMove = getMoveNotation(board, getNextMove());
		offset += sprintf_s(&pgn[offset], 24, "%d. %s ", board->fullmoveClock, whiteMove);
		nextMove();
		free(whiteMove);
	}
	

	pgn[offset] = '\0';


	return pgn;
}

void loadGameFromPGN(Board* board, char* PGN) {
	int offset = 0;

	char tagLine[200] = "";

	char FENString[150] = DefaultStart;
	
	int consumed;
	int result = sscanf_s(&PGN[offset], "[%[^]]%*[^\n]%n", tagLine, 200, &consumed);

	while (result > 0) {
		offset += consumed + 1;
		int _ = sscanf(tagLine, "FEN \"%[^\"]", FENString);
		result = sscanf_s(&PGN[offset], "[%[^]]%*[^\n]%n", tagLine, 200, &consumed);

	}

	loadFEN(FENString);

	int startOffset = offset;
	// clean up comments:
	int shiftOffset = offset;

	while (PGN[shiftOffset] != '\0') {

		if (PGN[shiftOffset] == '{') {
			int openings = 1;
			while (openings > 0) {
				if (PGN[++shiftOffset] == '\0')
					break;
				else if (PGN[shiftOffset] == '{')
					openings++;
				else if (PGN[shiftOffset] == '}')
					openings--;
			}
			++shiftOffset;
		}
		else if (PGN[shiftOffset] == '(') {
			int openings = 1;
			while (openings > 0) {
				if (PGN[++shiftOffset] == '\0')
					break;
				else if (PGN[shiftOffset] == '(')
					openings++;
				else if (PGN[shiftOffset] == ')')
					openings--;
			}
			++shiftOffset;
		}
		else if (PGN[shiftOffset] == ';') {
			while (PGN[++shiftOffset] != '\n' && PGN[shiftOffset] != '\0') {}
			++shiftOffset;
		}
		else {
			PGN[offset++] = PGN[shiftOffset++];
		}

	}
	PGN[offset] = '\0';

	offset = startOffset;
	//printf("%s\n", &PGN[offset]);

	int noteStart = offset;

	typedef enum State
	{
		Blank,
		Start,
		Reading,
	} State;

	State state = Blank;
	while (PGN[offset] != '\0') {

		switch (state)
		{
		case Blank:
			if (PGN[offset] == ' ' || PGN[offset] == '.')
				state = Start;
			break;
		case Start:
			if (!(PGN[offset] == ' ' || PGN[offset] == '.' || (PGN[offset] >= '0' && PGN[offset] <= '9')))
			{
				state = Reading;
				noteStart = offset;
			}
			break;
		case Reading:
			if (PGN[offset] == '.') {
				state = Start;
				break;
			}
			if (PGN[offset] == ' ' || PGN[offset] == '\n' || PGN[offset] == ';') {
				state = Start;
				PGN[offset] = '\0';
				Move move = getMoveFromNotation(board, &PGN[noteStart]);
				
				if (move != 0) {
					/*char* note = getMoveNotation(board, move);
					printf("%s -> %s\n", &PGN[noteStart], note);
					free(note);*/
					stashMove(move);
				}
			}
			break;
		default:
			break;
		}

		offset++;
	}
}