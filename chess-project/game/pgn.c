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