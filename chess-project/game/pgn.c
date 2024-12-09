#include "pgn.h"

#include "notations.h"

// Generate PGN string of the game
char* GetPGN(Board* board, GameData* gameState) {
    int moveCount = 0;
    while (gameState->prevEnabled) {
        PrevMove();
        moveCount++;
    }

    char* pgn = malloc(sizeof(char) * (130 + 12 * moveCount));

    if (pgn == NULL) {
        SDL_Log("Unable to allocate memory");
        exit(1);
    }

    int offset = sprintf(pgn, "[FEN \"%s\"]\n\n", gameState->startFEN);

    if (moveCount == 0) {
        offset += sprintf(&pgn[offset], "*");
        return pgn;
    }

    if (!board->isWhitesMove) {
        char* blackMove = GetMoveNotation(board, GetNextMove(gameState));
        offset += sprintf_s(&pgn[offset], 24, "%d... %s ", board->fullmoveClock, blackMove);
        NextMove(gameState);
        free(blackMove);
        moveCount--;
    }

    while (moveCount >= 2) {
        char* whiteMove = GetMoveNotation(board, GetNextMove(gameState));
        NextMove(gameState);
        char* blackMove = GetMoveNotation(board, GetNextMove(gameState));
        offset += sprintf_s(&pgn[offset], 24, "%d. %s %s ", board->fullmoveClock, whiteMove, blackMove);
        NextMove(gameState);
        free(whiteMove);
        free(blackMove);
        moveCount -= 2;
    }

    if (moveCount == 1) {
        char* whiteMove = GetMoveNotation(board, GetNextMove(gameState));
        offset += sprintf_s(&pgn[offset], 24, "%d. %s ", board->fullmoveClock, whiteMove);
        NextMove(gameState);
        free(whiteMove);
    }
    UpdateHasGameEnded(gameState);

    if (board->hasGameEnded) {
        if (board->winnerWhite) {
            offset += sprintf(&pgn[offset], "1-0");
        } else if (board->winnerBlack) {
            offset += sprintf(&pgn[offset], "0-1");
        } else {
            offset += sprintf(&pgn[offset], "1/2-1/2");
        }
    } else {
        offset += sprintf(&pgn[offset], "*");
    }

    pgn[offset] = '\0';


    return pgn;
}

// Load game from PGN string
void LoadGameFromPGN(Board* board, char* PGN, GameData* gameData) {
    int offset = 0;

    char tagLine[200] = "";

    char FENString[150] = DefaultBoardFEN;

    int consumed;
    int result = sscanf_s(&PGN[offset], "[%[^]]%*[^\n]%n", tagLine, 200, &consumed);

    while (result > 0) {
        offset += consumed + 1;
        int _ = sscanf(tagLine, "FEN \"%[^\"]", FENString);
        result = sscanf_s(&PGN[offset], "[%[^]]%*[^\n]%n", tagLine, 200, &consumed);
    }

    LoadFEN(FENString, gameData);

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
        } else if (PGN[shiftOffset] == '(') {
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
        } else if (PGN[shiftOffset] == ';') {
            while (PGN[++shiftOffset] != '\n' && PGN[shiftOffset] != '\0') {
            }
            ++shiftOffset;
        } else {
            PGN[offset++] = PGN[shiftOffset++];
        }
    }
    PGN[offset] = '\0';

    offset = startOffset;

    int noteStart = offset;

    typedef enum State {
        Blank,
        Start,
        Reading,
    } State;

    State state = Blank;
    while (PGN[offset] != '\0') {
        switch (state) {
            case Blank:
                if (PGN[offset] == ' ' || PGN[offset] == '.')
                    state = Start;
                break;
            case Start:
                if (!(PGN[offset] == ' ' || PGN[offset] == '.' || (PGN[offset] >= '1' && PGN[offset] <= '9'))) {
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

                    if ((strcmp(&PGN[noteStart], "*") == 0) ||
                        (strcmp(&PGN[noteStart], "1/2-1/2") == 0) ||
                        (strcmp(&PGN[noteStart], "1-0") == 0) ||
                        (strcmp(&PGN[noteStart], "0-1") == 0)) {
                        break;
                    }

                    Move move = GetMoveFromNotation(board, &PGN[noteStart]);

                    if (move != 0)
                        StashMove(move, gameData);
                }
                break;
            default:
                break;
        }
        offset++;
    }
}
