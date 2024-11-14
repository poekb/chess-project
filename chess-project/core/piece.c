#include "piece.h"

const int typeMask = 0b0111;
const int colourMask = 0b1000;

Piece MakePiece(PieceType type, Uint8 color) {
	return type | color;
}
Piece MakePieceIsWhite(PieceType type, bool IsWhite) {
	return MakePiece(type, IsWhite ? White : Black);
}

bool IsColour(Piece piece, Uint8 color) {
	return (piece & colourMask) == color;
}

bool IsWhite(Piece piece) {
	return IsColour(piece, White);
}

PieceType GetPieceType(Piece piece) {
	return typeMask & piece;
}

Piece PieceFromChar(char c) {
	bool IsWhite = isupper(c);
	if (IsWhite) c += 'a' - 'A';
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
	return MakePieceIsWhite(type, IsWhite);
}

char CharFromPiece(Piece piece) {
	PieceType type = GetPieceType(piece);
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

	return IsWhite(piece) ? toupper(c) : c;
}

Uint8 GetCode(Piece piece) {
	return piece - ((piece > 8) ? 1 : 3);
}