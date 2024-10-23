
#include "../global.h"

#ifndef PIECE
#define PIECE

typedef Uint8 Piece;
typedef Uint8 PieceType;

#endif

// Piece Types
#define None 0
#define Pawn 1
#define Knight 2
#define Bishop 3
#define Rook 4
#define Queen 5
#define King 6

// Piece Colors
#define White 0
#define Black 8

// White Pieces
#define WhitePawn 1      
#define WhiteKnight 2    
#define WhiteBishop 3   
#define WhiteRook 4
#define WhiteQueen 5
#define WhiteKing 6

//Black Pieces
#define BlackPawn 9
#define BlackKnight 10
#define BlackBishop 11
#define BlackRook 12
#define BlackQueen 13
#define BlackKing 14

Piece makePiece(PieceType type, Uint8 color);
Piece makePieceIsWhite(PieceType type, bool isWhite);
Piece pieceFromChar(char c);
char charFromPiece(Piece piece);

bool isColour(Piece piece, Uint8 color);

bool isWhite(Piece piece);

PieceType getPieceType(Piece piece);


