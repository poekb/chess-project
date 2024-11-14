#include "gameState.h"

bool HasKingsideCastleRight(GameState gameState, bool white){
	int mask = white ? 2 : 8;
	return (gameState.castleRights & mask) != 0;
}

bool HasQueensideCastleRight(GameState gameState, bool white){
	int mask = white ? 1 : 4;
	return (gameState.castleRights & mask) != 0;
}
