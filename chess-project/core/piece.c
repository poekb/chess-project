#include "piece.h"

const int typeMask = 0b0111;
const int colourMask = 0b1000;

Piece makePiece(PieceType type, Uint8 color) {
	return type | color;
}
Piece makePieceIsWhite(PieceType type, bool isWhite) {
	return makePiece(type, isWhite ? White : Black);
}

bool isColour(Piece piece, Uint8 color) {
	return (piece & colourMask) == color;
}

bool isWhite(Piece piece) {
	return isColour(piece, White);
}

PieceType getPieceType(Piece piece) {
	return typeMask & piece;
}

Piece pieceFromChar(char c) {
	bool isWhite = isupper(c);
	if (isWhite) c += 'a' - 'A';
	PieceType type = None;
	switch (c)
	{
	case 'p': type = Pawn; break;
	case 'n': type = Knight; break;
	case 'b': type = Bishop; break;
	case 'r': type = Rook; break;
	case 'q': type = Queen; break;
	case 'k': type = King; break;
	default: break;
	}
	return makePieceIsWhite(type, isWhite);
}

char charFromPiece(Piece piece) {
	PieceType type = getPieceType(piece);
	char c = ' ';
	switch (type)
	{
	case Pawn: c = 'p'; break;
	case Knight: c = 'n'; break;
	case Bishop: c = 'b'; break;
	case Rook: c = 'r'; break;
	case Queen: c = 'q'; break;
	case King: c = 'k'; break;
	default: break;
	}

	return isWhite(piece) ? toupper(c) : c;
}

Uint8 getCode(Piece piece) {
	return piece - ((piece > 8) ? 1 : 3);
}