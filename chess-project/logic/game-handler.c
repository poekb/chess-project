#include "game-handler.h"

#include "move-eval.h"

typedef struct Game {
	GamePosition position;

	Game* prev;
	Game* next;
} Game;

Game game;