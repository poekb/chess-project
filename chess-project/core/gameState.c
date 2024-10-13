#include "gameState.h"

const Uint8 ClearWhiteKingSide =  0b1101;
const Uint8 ClearWhiteQueenSide = 0b1110;
const Uint8 ClearBlackKingSide =  0b0111;
const Uint8 ClearBlackQueenSide = 0b1011;

bool hasKingsideCastleRight(GameState gameState, bool white){
	int mask = white ? 2 : 8;
	return (gameState.castleRights & mask) != 0;
}

bool hasQueensideCastleRight(GameState gameState, bool white){
	int mask = white ? 1 : 4;
	return (gameState.castleRights & mask) != 0;
}
