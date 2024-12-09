#include "notations.h"
#include "../moveGeneration/moveGenerator.h"
#include "../zobrist/zobristHashing.h"

// Get algebraic chess notation out of move
char* GetMoveNotation(Board* board, Move move) {
    char* string = malloc(sizeof(char) * 10);
    if (string == NULL) {
        SDL_Log("Unable to allocate memory");
        exit(1);
    }

    int length = 0;

    Move* moves = malloc(sizeof(Move) * 100);
    if (moves == NULL) return 0;
    int moveCount = GenerateMoves(board, moves, false);

    Uint8 start = GetStart(move);
    Uint8 target = GetTarget(move);

    PieceType type = GetPieceType(board->square[start]);

    if (IsCastle(move)) {
        string[length++] = '0';
        string[length++] = '-';
        string[length++] = '0';

        if (target % 8 == 2) // Queen side
        {
            string[length++] = '-';
            string[length++] = '0';
        }
        string[length] = '\0';
        return string;
    }

    switch (type) {
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

    bool capture = board->square[target] != None || IsEnPassantCapture(move);

    bool multiple = (capture) && (type == Pawn);
    bool sameFile = false;
    bool sameRank = false;

    for (int i = 0; i < moveCount; i++) {
        if (move == moves[i]) continue;
        Uint8 curStart = GetStart(moves[i]);

        if (target != GetTarget(moves[i]) || GetPieceType(board->square[curStart]) != type)
            continue;
        multiple = true;
        if (start % 8 == curStart % 8)
            sameFile = true;
        else if (start / 8 == curStart / 8)
            sameRank = true;
    }

    if (multiple) {
        if (!sameFile) {
            string[length++] = (start % 8) + 'a';
        } else if (!sameRank) {
            string[length++] = '8' - (start / 8);
        } else {
            string[length++] = (start % 8) + 'a';
            string[length++] = '8' - (start / 8);
        }
    }

    if (capture) {
        string[length++] = 'x';
    }

    string[length++] = (target % 8) + 'a';
    string[length++] = '8' - (target / 8);

    if (IsPromotion(move)) {
        string[length++] = '='; // Optional some engines don't use this

        if (move >> 12 == PromoteToQueenFlag) {
            string[length++] = 'Q';
        } else if (move >> 12 == PromoteToRookFlag) {
            string[length++] = 'R';
        } else if (move >> 12 == PromoteToBishopFlag) {
            string[length++] = 'B';
        } else if (move >> 12 == PromoteToKnightFlag) {
            string[length++] = 'N';
        }
    }

    MakeMove(board, move);
    moveCount = GenerateMoves(board, moves, false);
    bool isCheck = IsCheckPos(board);
    UnmakeMove(board, move);

    if (isCheck) {
        if (moveCount == 0) {
            string[length++] = '#';
        } else {
            string[length++] = '+';
        }
    }

    free(moves);

    string[length] = '\0';

    return string;
}

// Get a move from algebraic chess notation
Move GetMoveFromNotation(Board* board, char* notation) {
    char typeChar = notation[0];
    PieceType type = Pawn;

    Move* moves = malloc(sizeof(Move) * 100);
    if (moves == NULL) return 0;
    int moveCount = GenerateMoves(board, moves, false);

    switch (typeChar) {
        case 'K':
            type = King;
            break;
        case 'Q':
            type = Queen;
            break;
        case 'R':
            type = Rook;
            break;
        case 'B':
            type = Bishop;
            break;
        case 'N':
            type = Knight;
            break;
        case 'O':
        case '0': {
            //castling
            bool queenSide = (notation[3] == '-');
            board->currentGameState.castleRights;
            for (int i = 0; i < moveCount; i++) {
                Move move = moves[i];
                if (IsCastle(move)) {
                    Uint8 target = GetTarget(move);
                    Uint8 targetFile = target % 8;

                    if (targetFile == (queenSide ? 2 : 6)) {
                        free(moves);
                        return move;
                    }
                }
            }
            free(moves);
            return 0;
        }
    }

    int ptr = (int) strlen(notation);
    while (!isalnum(notation[--ptr])) {
    }

    int promotion = 0;

    if (isupper(notation[ptr])) {
        //promotion
        switch (notation[ptr]) {
            case 'Q':
                promotion = PromoteToQueenFlag;
                break;
            case 'R':
                promotion = PromoteToRookFlag;
                break;
            case 'B':
                promotion = PromoteToBishopFlag;
                break;
            case 'N':
                promotion = PromoteToKnightFlag;
                break;
        }
        if (notation[--ptr] == '=') ptr--;
    }
    Uint8 targetRank = 8 - (notation[ptr--] - '0');
    Uint8 targetFile = notation[ptr--] - 'a';

    Uint8 targetSquare = targetRank * 8 + targetFile;

    bool isCapture = false;
    if (notation[ptr] == 'x') {
        isCapture = true;
        ptr--;
    }

    bool startRankPresent = false;
    bool startFilePresent = false;
    Uint8 startRank = 0;
    Uint8 startFile = 0;

    if (ptr >= 0 && isdigit(notation[ptr]))
        startRankPresent = true, startRank = 8 - (notation[ptr--] - '0');
    if (ptr >= 0 && islower(notation[ptr]))
        startFilePresent = true, startFile = notation[ptr--] - 'a';


    for (int i = 0; i < moveCount; i++) {
        Move move = moves[i];
        Uint8 target = GetTarget(move);
        Uint8 moveStart = GetStart(move);
        Uint8 moveStartRank = moveStart / 8;
        Uint8 moveStartFile = moveStart % 8;


        if ((target == targetSquare) && (GetPieceType(board->square[moveStart]) == type)
            && (!startRankPresent || (moveStartRank == startRank))
            && (!startFilePresent || (moveStartFile == startFile))
            && (((promotion == 0) && !IsPromotion(move)) || ((move >> 12) == promotion))
        ) {
            free(moves);
            return move;
        }
    }
    free(moves);
    return 0;
}

// Get FEN from a position
int GetFENFromBoard(Board* board, char* FEN) {
    int length = 0;

    for (int i = 0; i < 8; i++) {
        int sum = 0;
        for (int j = 0; j < 8; j++) {
            Piece piece = board->square[i * 8 + j];
            if (piece == None) {
                sum++;
            } else {
                if (sum != 0)
                    FEN[length++] = ('0' + sum);
                sum = 0;
                FEN[length++] = CharFromPiece(piece);
            }
        }
        if (sum != 0)
            FEN[length++] = ('0' + sum);
        if (i < 7)
            FEN[length++] = '/';
    }
    FEN[length++] = ' ';
    FEN[length++] = (board->isWhitesMove ? 'w' : 'b');
    FEN[length++] = ' ';

    Uint8 castleRights = board->currentGameState.castleRights;
    if (castleRights > 0) {
        if ((castleRights & CastleWhiteKingSide) != 0) FEN[length++] = 'K';
        if ((castleRights & CastleWhiteQueenSide) != 0) FEN[length++] = 'Q';
        if ((castleRights & CastleBlackKingSide) != 0) FEN[length++] = 'k';
        if ((castleRights & CastleBlackQueenSide) != 0) FEN[length++] = 'q';
    } else {
        FEN[length++] = '-';
    }

    FEN[length++] = ' ';

    Uint8 enPassantFile = board->currentGameState.enpassantFile;
    if (enPassantFile == (Uint8) -1) {
        FEN[length++] = '-';
    } else {
        char file = 'a' + enPassantFile;

        FEN[length++] = file;
    }

    int result = sprintf(&FEN[length], " %d %d", board->currentGameState.halfmoveClock, board->fullmoveClock);
    length += result;

    FEN[length] = '\0';

    return length;
}

// Load a position from a FEN string
void LoadBoardFromFEN(Board* board, char* FENString) {
    board->zobristHash = 0;

    int strPointer = 0;
    int pointer = 0;
    while (pointer < 64) {
        char c = FENString[strPointer];
        if ('0' < c && c < '9') {
            for (int i = 1; i < c - '0'; i++) {
                board->square[pointer++] = None;
            }
            pointer++;
        } else if (c != '/') {
            Piece piece = PieceFromChar(c);
            board->square[pointer] = piece;
            int colorIndex = IsWhite(piece) ? WhiteIndex : BlackIndex;
            PieceType type = GetPieceType(piece);

            MakePieceAtSquare(board, pointer, type, colorIndex);

            pointer++;
        }
        strPointer++;
    }

    strPointer++;
    board->isWhitesMove = (FENString[strPointer] == 'w');
    if (!(board->isWhitesMove)) board->zobristHash ^= zobristBlacksTurn;
    board->currentGameState.capturedPiece = None;
    strPointer++;

    board->currentGameState.castleRights = 0;

    if (FENString[strPointer] != '-') {
        while (FENString[++strPointer] != ' ') {
            switch (FENString[strPointer]) {
                case 'k':
                    board->currentGameState.castleRights |= CastleBlackKingSide;
                    break;
                case 'q':
                    board->currentGameState.castleRights |= CastleBlackQueenSide;
                    break;
                case 'K':
                    board->currentGameState.castleRights |= CastleWhiteKingSide;
                    break;
                case 'Q':
                    board->currentGameState.castleRights |= CastleWhiteQueenSide;
                    break;
                default: break;
            }
        }
    }

    board->zobristHash ^= zobristCastlingRights[board->currentGameState.castleRights];

    strPointer++;

    if (FENString[strPointer] != '-') {
        board->currentGameState.enpassantFile = (FENString[strPointer] - 'a');
        if (isdigit(FENString[strPointer + 1])) strPointer++;
        //If they provide the en passant rank we discard it, because it is unnecessary

        board->zobristHash ^= zobristEnpassant[board->currentGameState.enpassantFile];
    } else
        board->currentGameState.enpassantFile = -1;

    strPointer++;
    int halfMove, fullMove;
    int result = sscanf(&FENString[strPointer], "%d %d", &halfMove, &fullMove);

    if (result < 2) {
        halfMove = 0;
        fullMove = 1;
    }

    board->currentGameState.halfmoveClock = (Uint8) halfMove;
    board->fullmoveClock = (Uint16) fullMove;

    board->gameStateHistoryCount = 0;

    board->hasGameEnded = false;
    board->winnerWhite = false;
    board->winnerBlack = false;
}
