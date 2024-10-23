#include "gameState.h"

bool hasKingsideCastleRight(GameState gameState, bool white){
	int mask = white ? 2 : 8;
	return (gameState.castleRights & mask) != 0;
}

bool hasQueensideCastleRight(GameState gameState, bool white){
	int mask = white ? 1 : 4;
	return (gameState.castleRights & mask) != 0;
}
